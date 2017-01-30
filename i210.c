#include "i210.h"
#include "i210_reg.h"

static void i210_enable_tx_rx_interrutps(struct i210_prv_data *pdata)
{
	int reg = 0;
	
	/* TXDW, RXDMT0, RX miss, RXDW*/
	reg = (BIT(0) | BIT(4) | BIT(6) | BIT(7));
	i210_writel(reg, IMS);
	i210_writel(reg, ICS);
}

static void i210_disable_tx_rx_interrutps(struct i210_prv_data *pdata)
{
	i210_writel(0, IMS);
	i210_writel(0, ICS);
}

irqreturn_t i210_isr_handler(int irq, void *device_id)
{
	struct i210_prv_data *pdata = (struct i210_prv_data *)device_id;
	int reg;

	reg = i210_readl(ICR);

	printk(KERN_ALERT "ISR: ICR:%#x\n", reg);

	if (reg & (BIT(0) | BIT (7))) {
		i210_disable_tx_rx_interrutps(pdata);
		__napi_schedule(&pdata->napi);
	}
	
	return IRQ_HANDLED;
}

static void i210_prepare_rx_desc(int qidx, struct i210_prv_data *pdata, int didx)
{
	struct rx_desc_wrap *wrx_desc = &pdata->wrxdesc[qidx];
	struct rx_buf *rxbuf = &wrx_desc->rxbuf[didx];
	struct rx_desc *rxdesc = wrx_desc->rxdesc[didx];

	rxdesc->buf_addr = rxbuf->buf_dma;
}

static void i210_prepare_tx_desc(int qidx, struct i210_prv_data *pdata, int didx)
{
	struct tx_desc_wrap *wtx_desc = &pdata->wtxdesc[qidx];
	struct tx_buf *txbuf = &wtx_desc->txbuf[didx];
	struct tx_desc *txdesc = wtx_desc->txdesc[didx];
	u64 dw1 = 0;

	/* Descriptor setup to xmit the data */
	txdesc->buf_addr = txbuf->buf_dma;
	dw1 = (txbuf->len & 0xff);
	/* CMD: Setting EOP, IFCS, RS */
	dw1 |= (BIT(24) | BIT(25) | BIT(27));
	/* STA: Setting DD */
	dw1 |= BIT(32);
	txdesc->dw1 = dw1;

}



static int i210_realloc_skb(int qidx, struct i210_prv_data *pdata)
{	
	struct rx_desc_wrap *wrx_desc = &pdata->wrxdesc[qidx];
	struct rx_buf *rxbuf = NULL;
	struct sk_buff *skb = NULL;
	int i;

	for(i = 0; i < wrx_desc->dirty_rx; i++) {
		rxbuf = &wrx_desc->rxbuf[wrx_desc->realloc_idx];
		skb = rxbuf->skb;

		skb = dev_alloc_skb(RX_BUF_SIZE + NET_IP_ALIGN);
		if (skb == NULL) {
			HANDLE_ERROR();
		}
		skb_reserve(skb, NET_IP_ALIGN);

		rxbuf->skb = skb;
		rxbuf->len = RX_BUF_SIZE;

		rxbuf->buf_dma = dma_map_single(&pdata->pdev->dev, skb->data, RX_BUF_SIZE, DMA_FROM_DEVICE);
		if (dma_mapping_error(&pdata->pdev->dev, rxbuf->buf_dma))
			printk(KERN_ALERT "failed to do the RX dma map\n");

		wmb();

		i210_prepare_rx_desc(qidx, pdata, wrx_desc->realloc_idx);
		i210_writel(wrx_desc->realloc_idx, RDT(qidx));
		wrx_desc->realloc_idx++;
		if (wrx_desc->realloc_idx >= RX_DESC_CNT) {
			wrx_desc->realloc_idx -= RX_DESC_CNT;
		}
	}

	return 0;
}

static int i210_clean_tx_data(int qidx, struct i210_prv_data *pdata, int quota)
{
	HANDLE_ME();
	return 0;
}

static int i210_clean_rx_data(int qidx, struct i210_prv_data *pdata, int quota)
{
	struct rx_desc_wrap *wrx_desc = &pdata->wrxdesc[qidx];
	int index = wrx_desc->cur_rx;
	struct rx_desc *rx_desc = wrx_desc->rxdesc[index];
	struct net_device *netdev = pdata->netdev;
	struct rx_buf *rxbuf = &wrx_desc->rxbuf[index];
	struct sk_buff *skb = rxbuf->skb;
	int pkt_len;
	int dd, eop, received = 0;

	while (received < quota) {
		dd = ((rx_desc->dw1 >> 32) & 1);
		eop = ((rx_desc->dw1 >> 33) & 1);
		pkt_len = rx_desc->dw1 & 0xff;

		if(dd & eop) {
			skb_put(skb, pkt_len);
			netdev->last_rx = jiffies;
			/* update the statistics */
			netdev->stats.rx_packets++;
			netdev->stats.rx_bytes += skb->len;
			netif_receive_skb(skb);
			received ++;
			HANDLE_ME();
		} else {
			break;
		}

		wrx_desc->dirty_rx++;
		if (wrx_desc->dirty_rx >= TX_DESC_CNT) {
			wrx_desc->dirty_rx -= TX_DESC_CNT;
		}
		if (wrx_desc->dirty_rx >= RX_DESC_MIN_CNT) {
			i210_realloc_skb(qidx, pdata);
		}
	}

	return 0;
}

int i210_poll(struct napi_struct *napi, int budget)
{
	struct i210_prv_data *pdata = 
		container_of(napi, struct i210_prv_data, napi);
	int i;

	if (!pdata)
		printk(KERN_ALERT "NULL PDATA\n");

	for (i = 0; i < RX_Q_CNT; i++) {
		i210_clean_rx_data(i, pdata, budget/RX_Q_CNT);
	}

	for (i = 0; i < TX_Q_CNT; i++) {
		i210_clean_tx_data(i, pdata, budget/TX_Q_CNT);
	}

	i210_enable_tx_rx_interrutps(pdata);
	
	return 0;
}

static netdev_tx_t i210_xmit(struct sk_buff *skb,
				    struct net_device *netdev)
{
	struct i210_prv_data *pdata = netdev_priv(netdev);
	unsigned int qidx = skb_get_queue_mapping(skb);
	struct tx_desc_wrap *wtx_desc = &pdata->wtxdesc[qidx];
	struct tx_buf *txbuf = &wtx_desc->txbuf[wtx_desc->cur_tx];
	int count = 0, len;

	if (skb->len <= 0) {
		dev_kfree_skb_any(skb);
		printk(KERN_ERR "%s : Empty skb received from stack\n",
		       netdev->name);
		return NETDEV_TX_OK;
	}

	if ((skb->len > MAX_PACKET_SIZE)) {
		printk(KERN_ERR "%s : big packet = %d\n", netdev->name,
		       (u16)skb->len);
		dev_kfree_skb_any(skb);
		netdev->stats.tx_dropped++;
		return NETDEV_TX_OK;
	}

	spin_lock(&pdata->tx_lock);

	if (wtx_desc->tx_free_cnt == 0) {
		HANDLE_ME();
	}

	len = max((skb->len - skb->data_len), (unsigned int)ETH_ZLEN);
	txbuf->skb = skb;
	txbuf->buf_dma = dma_map_single((&pdata->pdev->dev),
					skb->data, len, DMA_TO_DEVICE);
	if (dma_mapping_error((&pdata->pdev->dev), txbuf->buf_dma)) {
		printk(KERN_ALERT "failed to do the dma map\n");
		HANDLE_ME();
		return -ENOMEM;
	}
	txbuf->len = len;
	/* TODO:Handle fragmented packets */
	count = 1;

	netdev->trans_start = jiffies;
	wtx_desc->tx_free_cnt -= count;
	wtx_desc->tx_queued += count;
	skb_tx_timestamp(skb);

	i210_prepare_tx_desc(qidx, pdata, wtx_desc->cur_tx);

	wtx_desc->cur_tx++;
	if (wtx_desc->cur_tx >= TX_DESC_CNT) {
		wtx_desc->cur_tx -= TX_DESC_CNT;
	}

	/* Updating the tail pointer */
	i210_writel(wtx_desc->cur_tx, TDT(qidx));

	spin_unlock(&pdata->tx_lock);

	return NETDEV_TX_OK;
}

static void i210_adjust_link(struct net_device *netdev)
{
	struct i210_prv_data *pdata = netdev_priv(netdev);
	struct i210_phy_data *phy_data = &pdata->phy_data;
	struct phy_device *phydev = phy_data->phy_dev;

	if (phydev == NULL)
		return;

	if (phydev->link) {
		if (phydev->duplex != phy_data->oldduplex) {
			if (phydev->duplex) {
				//Full duplex
			}
			else {
				//half duplex
			}
			phy_data->oldduplex = phydev->duplex;
		}

		if (phydev->speed != phy_data->link_speed) {
			switch (phydev->speed) {
			case SPEED_1000:
				// gmii speed 1000
				break;
			case SPEED_100:
				// 100
				break;
			case SPEED_10:
				// 10
				break;
			}
			phy_data->link_speed = phydev->speed;
		}

		if (!phy_data->oldlink) {
			phy_data->oldlink = 1;
		}
	} else if (phy_data->oldlink) {
		phy_data->oldlink = 0;
		phy_data->link_speed = 0;
		phy_data->oldduplex = -1;
	}

}

static int i210_init_phy(struct net_device *netdev)
{
	struct i210_prv_data *pdata = netdev_priv(netdev);
	struct i210_phy_data *phy_data = &pdata->phy_data;
	struct phy_device *phydev = NULL;
	char phy_id_fmt[MII_BUS_ID_SIZE + 3];
	char bus_id[MII_BUS_ID_SIZE];

	phy_data->oldlink = 0;
	phy_data->link_speed = 0;
	phy_data->oldduplex = -1;
	snprintf(bus_id, MII_BUS_ID_SIZE, "i210_phy-%x", phy_data->bus_id);
	snprintf(phy_id_fmt, MII_BUS_ID_SIZE + 3, PHY_ID_FMT, bus_id,
		 phy_data->phy_addr);

	printk(KERN_ALERT "trying to attach to %s\n", phy_id_fmt);
	phydev = phy_connect(netdev, phy_id_fmt, &i210_adjust_link, PHY_INTERFACE_MODE_GMII);
	if (IS_ERR(phydev)) {
		printk(KERN_ALERT "%s: Could not attach to PHY\n", netdev->name);
		return PTR_ERR(phydev);
	}
	phydev->supported &= PHY_GBIT_FEATURES;
	phydev->advertising = phydev->supported;

	if (phydev->phy_id == 0) {
		phy_disconnect(phydev);
		return -ENODEV;
	}
	printk(KERN_ALERT "%s: attached to PHY (UID 0x%x) Link = %d\n",
	       netdev->name, phydev->phy_id, phydev->link);

	phy_data->phy_dev = phydev;
	phy_start(phy_data->phy_dev);

	return 0;
}

int i210_phy_reg_read(struct i210_prv_data *pdata, int phyaddr,
			    int phyreg, int *phydata)
{
	int mdic, mdicnfg;

	mdicnfg = i210_readl(MDICNFG);
	mdicnfg = phyaddr << 21;
	i210_writel(mdicnfg, MDICNFG);

	mdic = i210_readl(MDIC);
	mdic = (phyreg & 0x1f) << 16;
	/* Opcode for read */
	mdic |= (2 << 26);
	/* Ready bit cleared explicitly */
	mdic &= ~(BIT(28));
	i210_writel(mdic, MDIC);

	while(!(mdic & BIT(28))) {
		mdic = i210_readl(MDIC);
	}

	/* ERROR */
	if (mdic & BIT(30)) {
		return -1;
	}

	*phydata = (mdic & 0xff);

	return 0;
}

int i210_mdio_read_direct(struct i210_prv_data *pdata, int phyaddr,
			    int phyreg, int *phydata)
{
	return i210_phy_reg_read(pdata, phyaddr, phyreg, phydata);
}


static int i210_mdio_read(struct mii_bus *bus, int phyaddr, int phyreg)
{
	struct net_device *netdev = bus->priv;
	struct i210_prv_data *pdata = netdev_priv(netdev);
	int phydata;

	if (i210_phy_reg_read(pdata, phyaddr, phyreg, &phydata) != 0) {
		printk(KERN_ALERT "PHY reg read failed\n");
		return -1;
	}

	return phydata;
}

static int i210_mdio_write(struct mii_bus *bus, int phyaddr,
			   int phyreg, u16 phydata)
{
	struct net_device *netdev = bus->priv;
	struct i210_prv_data *pdata = netdev_priv(netdev);

	int mdic, mdicnfg;

	mdicnfg = i210_readl(MDICNFG);
	mdicnfg = phyaddr << 21;
	i210_writel(mdicnfg, MDICNFG);

	mdic = i210_readl(MDIC);
	mdic = (phyreg & 0x1f) << 16;
	/* Opcode for write */
	mdic |= (1 << 26);
	mdic &= ~(0xff);
	mdic |= phydata;
	i210_writel(mdic, MDIC);

	while(!(mdic & BIT(28))) {
		mdic = i210_readl(MDIC);
	}

	/* ERROR */
	if (mdic & BIT(30)) {
		return -1;
	}

	return 0;
}

static int i210_mdio_reset(struct mii_bus *bus)
{
	struct net_device *netdev = bus->priv;
	struct i210_prv_data *pdata = netdev_priv(netdev);
	int reg;

	printk(KERN_ALERT "Entering while @ %d\n", __LINE__);
	while(i210_readl(MANC) & BIT(18));
	printk(KERN_ALERT "Exiting while @ %d\n", __LINE__);

	do {
		reg = i210_readl(SWSM);
		reg |= BIT(1);
		i210_writel(reg, SWSM);
		udelay(1);
		reg = i210_readl(SWSM);
		/* If SWSM.SWESMBI was successfully set (semaphore was acquired); otherwise, go back */
	} while (!(reg & BIT(1)));

	/* When bit 17 set to 1b, PHY access is owned by firmware:
	 * If the bit is set (firmware owns the PHY), software tries again later
	 * wait for it to clear for software access */
	//TODO: Check this later

	printk(KERN_ALERT "Entering while @ %d\n", __LINE__);
	while (i210_readl(SW_FW_SYNC) & BIT(17));
	printk(KERN_ALERT "Exiting while @ %d\n", __LINE__);

	reg = i210_readl(SWSM);
	reg &= ~(BIT(1));
	i210_writel(reg, SWSM);

	reg = i210_readl(CTRL);
	reg |= (BIT(31));
	i210_writel(reg, CTRL);

	udelay(100);

	reg &= ~(BIT(31));
	i210_writel(reg, CTRL);

	do {
		reg = i210_readl(SWSM);
		reg |= BIT(1);
		i210_writel(reg, SWSM);
		udelay(1);
		reg = i210_readl(SWSM);
		/* If SWSM.SWESMBI was successfully set (semaphore was acquired); otherwise, go back */
	} while (!(reg & BIT(1)));

	reg = i210_readl(SW_FW_SYNC);
	reg &= ~(BIT(17));
	i210_writel(reg, SW_FW_SYNC);

	reg = i210_readl(SWSM);
	reg &= ~(BIT(1));
	i210_writel(reg, SWSM);

	printk(KERN_ALERT "Entering while @ %d\n", __LINE__);
	while(!(i210_readl(EEMNGCTL) & BIT(18)));
	printk(KERN_ALERT "Exiting while @ %d\n", __LINE__);

	return 0;
}


static void i210_dump_phy_registers(struct i210_prv_data *pdata)
{
	int phydata = 0;

	printk(KERN_ALERT "\n************* PHY Reg dump *****************\n");
	i210_mdio_read_direct(pdata, pdata->phy_data.phy_addr, MII_BMCR, &phydata);
	printk(KERN_ALERT "Basic Mode Control Reg(%#x) = %#x\n", MII_BMCR,
	       phydata);

	i210_mdio_read_direct(pdata, pdata->phy_data.phy_addr, MII_BMSR, &phydata);
	printk(KERN_ALERT "Basic Mode Status Reg(%#x) = %#x\n", MII_BMSR,
	       phydata);

	i210_mdio_read_direct(pdata, pdata->phy_data.phy_addr, MII_PHYSID1, &phydata);
	printk(KERN_ALERT "PHYS ID 1(%#x) = %#x\n", MII_PHYSID1, phydata);

	i210_mdio_read_direct(pdata, pdata->phy_data.phy_addr, MII_PHYSID2, &phydata);
	printk(KERN_ALERT "PHYS ID 2(%#x) = %#x\n", MII_PHYSID2, phydata);

	i210_mdio_read_direct(pdata, pdata->phy_data.phy_addr, MII_ADVERTISE, &phydata);
	printk(KERN_ALERT "Advertisement Control Reg(%#x) = %#x\n",
	       MII_ADVERTISE, phydata);

	/* read Phy Control Reg */
	i210_mdio_read_direct(pdata, pdata->phy_data.phy_addr, MII_LPA, &phydata);
	printk(KERN_ALERT "Link Partner Ability Reg(%#x) = %#x\n", MII_LPA,
	       phydata);

	i210_mdio_read_direct(pdata, pdata->phy_data.phy_addr, MII_EXPANSION, &phydata);
	printk(KERN_ALERT "Extension Reg(%#x) = %#x\n", MII_EXPANSION, phydata);

	i210_mdio_read_direct(pdata, pdata->phy_data.phy_addr, AUTO_NEGO_NP, &phydata);
	printk(KERN_ALERT "Auto-nego Np(%#x)= %#x\n", AUTO_NEGO_NP, phydata);

	i210_mdio_read_direct(pdata, pdata->phy_data.phy_addr, MII_ESTATUS, &phydata);
	printk(KERN_ALERT "Extended Status Reg(%#x) = %#x\n", MII_ESTATUS,
	       phydata);

	i210_mdio_read_direct(pdata, pdata->phy_data.phy_addr, MII_CTRL1000, &phydata);
	printk(KERN_ALERT "1000BASE-T Control Reg(%#x) = %#x\n", MII_CTRL1000,
	       phydata);

	i210_mdio_read_direct(pdata, pdata->phy_data.phy_addr, MII_STAT1000, &phydata);
	printk(KERN_ALERT "1000BASE-T Status(%#x) = %#x\n", MII_STAT1000,
	       phydata);

	i210_mdio_read_direct(pdata, pdata->phy_data.phy_addr, PHY_CTL, &phydata);
	printk(KERN_ALERT "PHY Ctl Reg(%#x) = %#x\n", PHY_CTL, phydata);

	i210_mdio_read_direct(pdata, pdata->phy_data.phy_addr, PHY_STS, &phydata);
	printk(KERN_ALERT "PHY Sts Reg(%#x) = %#x\n", PHY_STS, phydata);

	printk(KERN_ALERT "\n******************************************\n");
}

static int i210_init_mdio(struct net_device *netdev)
{
	struct i210_prv_data *pdata = netdev_priv(netdev);
	struct mii_bus *new_bus = NULL;
	int phyaddr = 0, ret = 0;
	unsigned short phy_detected = 0;
	struct i210_phy_data *phy_data = &pdata->phy_data;


	/* find the phy ID or phy address which is connected to our MAC */
	for (phyaddr = 0; phyaddr < 32; phyaddr++) {
		int phy_reg_read_status, mii_status;

		phy_reg_read_status = i210_mdio_read_direct(
		    pdata, phyaddr, MII_BMSR, &mii_status);
		printk(KERN_ALERT "phy_status=%d", phy_reg_read_status);
		if (phy_reg_read_status == 0) {
			if (mii_status != 0x0000 && mii_status != 0xffff) {
				printk(KERN_ALERT
				       "%s: Phy detected at ID/ADDR %d\n",
				       DEV_NAME, phyaddr);
				phy_detected = 1;
				break;
			}
		} else if (phy_reg_read_status < 0) {
			printk(KERN_ALERT
			       "%s: Error reading the phy register MII_BMSR "
			       "for phy ID/ADDR %d\n",
			       DEV_NAME, phyaddr);
		}
	}
	if (!phy_detected) {
		printk(KERN_ALERT "%s: No phy could be detected\n", DEV_NAME);
		return -ENOLINK;
	}
	phy_data->phy_addr = phyaddr;

	phy_data->bus_id = 0x1;
	new_bus = mdiobus_alloc();
	if (new_bus == NULL) {
		printk(KERN_ALERT "Unable to allocate mdio bus\n");
		return -ENOMEM;
	}
	new_bus->name = "i210_phy";
	new_bus->read = i210_mdio_read;
	new_bus->write = i210_mdio_write;
	new_bus->reset = i210_mdio_reset;
	snprintf(new_bus->id, MII_BUS_ID_SIZE, "%s-%x", new_bus->name,
		 phy_data->bus_id);
	new_bus->priv = netdev;
	new_bus->phy_mask = 0;
	new_bus->parent = &pdata->pdev->dev;

	ret = mdiobus_register(new_bus);
	if (ret != 0) {
		printk(KERN_ALERT "%s: Cannot register as MDIO bus\n",
		       new_bus->name);
		mdiobus_free(new_bus);
		return ret;
	}
	phy_data->miibus = new_bus;

	ret = i210_init_phy(netdev);
	if (unlikely(ret)) {
		printk(KERN_ALERT "Cannot attach to PHY (error: %d)\n", ret);
		return ret;
	}

	i210_dump_phy_registers(pdata);

	return 0;
}

static int i210_init_tx_desc(struct i210_prv_data *pdata, int qidx)
{
	struct tx_desc_wrap *wtx_desc = &pdata->wtxdesc[qidx];
	int reg;

	HANDLE_ME();

	/* Disable the queue */
	reg = i210_readl(TXDCTL(qidx));
	reg &= ~(BIT(25));
	i210_writel(reg, TXDCTL(qidx));

	i210_writel((wtx_desc->txdesc_dma[0] & 0xffffffff), TDBAL(qidx));
	i210_writel(((wtx_desc->txdesc_dma[0] >> 32) & 0xffffffff), TDBAH(qidx));
	i210_writel((sizeof(struct tx_desc) * TX_DESC_CNT), TDLEN(qidx));
	/* Program the TXDCTL register with the desired Tx descriptor write back policy. Suggested values
	are:
	— WTHRESH = 1b
	— All other fields 0b */
	/* If needed, set TDWBAL/TWDBAH to enable head write back.*/
	reg = i210_readl(TXDCTL(qidx));
	reg &= ~((0x1f << 16));
	/* Making WTHRESH = 1 */
	reg |= BIT(16);
	i210_writel(reg, TXDCTL(qidx));

	i210_writel(0, TDH(qidx));
	i210_writel(0, TDT(qidx));

	/* Enable the queue using TXDCTL.ENABLE (queue zero is enabled by default). */
	reg = i210_readl(TXDCTL(qidx));
	reg |= BIT(25);
	i210_writel(reg, TXDCTL(qidx));

	/* Poll the TXDCTL register until the ENABLE bit is set */
	while (i210_readl(TXDCTL(qidx)) & BIT(25));

	/* Setting EN bit */
	reg = i210_readl(TCTL);
	reg |= BIT(1) | BIT(24);
	i210_writel(reg, TCTL);

	return 0;
}


static int i210_init_rx_desc(struct i210_prv_data *pdata, int qidx)
{
	struct rx_desc_wrap *wrx_desc = &pdata->wrxdesc[qidx];
	int reg;

	HANDLE_ME();
	reg = i210_readl(RXDCTL(qidx));
	reg &= ~(BIT(25));
	i210_writel(reg, RXDCTL(qidx));

	i210_writel((wrx_desc->rxdesc_dma[0] & 0xffffffff), RDBAL(qidx));
	i210_writel(((wrx_desc->rxdesc_dma[0] >> 32) & 0xffffffff), RDBAH(qidx));
	i210_writel((sizeof(struct rx_desc) * RX_DESC_CNT), RDLEN(qidx));
	/* Program SRRCTL - Currently not required*/
	/* program PSRTYPE for split header - Currently not required*/
	i210_writel(0, RDH(qidx));
	//TODO:verify later
	i210_writel(RX_DESC_CNT, RDT(qidx));

	/* For queue 0 ring parameters should be set before RCTL.RXEN is set */
	reg = i210_readl(RCTL);
	/* RXEN */
	reg |= BIT(1);
	i210_writel(reg, RCTL);

	/* Enable the queue by setting RXDCTL.ENABLE */
	reg = i210_readl(RXDCTL(qidx));
	reg |= BIT(25);
	i210_writel(reg, RXDCTL(qidx));

	/* Poll the RXDCTL register until the ENABLE bit is set */
	while (i210_readl(RXDCTL(qidx)) & BIT(25));

	/* Program the direction of packets to this queue according to the mode selected in the MRQC register */
	/* The tail register of the queue (RDT[n]) should not be bumped until the queue is enabled */

	return 0;
}
static int i210_alloc_tx_desc_oneq(struct i210_prv_data *pdata, int qidx)
{
	struct tx_desc_wrap *wtx_desc = &pdata->wtxdesc[qidx];
	char *tx_desc_ptr = NULL;
	int i, err;
	dma_addr_t txdesc_dma_tmp;

	wtx_desc->cur_tx = 0;
	wtx_desc->durty_tx = 0;
	wtx_desc->tx_queued = 0;

	tx_desc_ptr = (char *)dma_alloc_coherent(&pdata->pdev->dev,
						 ((sizeof(struct tx_desc)) *
						  TX_DESC_CNT),
					       &txdesc_dma_tmp,
					       GFP_KERNEL);
	if (!tx_desc_ptr) {
		printk(KERN_ALERT "TX desc allocation failed\n");
		return -ENOMEM;
	}

	for (i = 0; i < TX_DESC_CNT; i++) {
		wtx_desc->txdesc[i] = tx_desc_ptr + sizeof(struct tx_desc);
		wtx_desc->txdesc_dma[i] = txdesc_dma_tmp +
			sizeof(struct tx_desc);
	}

	wtx_desc->tx_free_cnt = TX_DESC_CNT;

	err = i210_init_tx_desc(pdata, qidx);
	if (err != 0)
		return err;

	return 0;
}

static int i210_alloc_tx_desc(struct i210_prv_data *pdata)
{
	struct tx_desc_wrap *wtx_desc_ptr = NULL;
	int  i, ret = 0;

	wtx_desc_ptr = kzalloc((TX_Q_CNT * sizeof(const struct tx_desc_wrap))
				  ,GFP_KERNEL);
	if (wtx_desc_ptr) {
		printk(KERN_ALERT "tx_wrapper desc failed\n");
		return -ENOMEM;
	}
	pdata->wtxdesc = wtx_desc_ptr;

	for (i = 0; i < TX_Q_CNT; i++) {
		ret = i210_alloc_tx_desc_oneq(pdata, i);
		if (ret != 0) {
			goto fail;
		}
	}

	return 0;
fail:
	kfree(wtx_desc_ptr);
	pdata->wtxdesc = NULL;

	return -ENOMEM;
}


static int i210_alloc_rx_desc_oneq(struct i210_prv_data *pdata, int qidx)
{
	struct rx_desc_wrap *wrx_desc = &pdata->wrxdesc[qidx];
	char *rx_desc_ptr = NULL;
	dma_addr_t rxdesc_dma_tmp;
	struct sk_buff *skb;
	int i = 0, err = 0;

	wrx_desc->cur_rx = 0;
	wrx_desc->dirty_rx = 0;
	wrx_desc->rx_queued = 0;
	wrx_desc->realloc_idx = 0;
	
	rx_desc_ptr = (char *)dma_alloc_coherent(&pdata->pdev->dev,((sizeof(struct rx_desc)) * RX_DESC_CNT),
					       &rxdesc_dma_tmp,
					       GFP_KERNEL);
	if (!rx_desc_ptr) {
		printk(KERN_ALERT "RX desc allocation failed\n");
		return -ENOMEM;
	}

	for (i = 0; i < RX_DESC_CNT; i++) {
		wrx_desc->rxdesc[i] = rx_desc_ptr + sizeof(struct rx_desc);
		wrx_desc->rxdesc_dma[i] = rxdesc_dma_tmp + sizeof(struct rx_desc);
		/* allocate skb & assign to each desc */
		skb = dev_alloc_skb(RX_BUF_SIZE + NET_IP_ALIGN);
		if (skb == NULL) {
			HANDLE_ERROR();
			break;
		}
		skb_reserve(skb, NET_IP_ALIGN);

		wrx_desc->rxbuf[i].skb = skb;
		wrx_desc->rxbuf[i].buf_dma = dma_map_single(&pdata->pdev->dev, skb->data, RX_BUF_SIZE, DMA_FROM_DEVICE);
		wrx_desc->rxbuf[i].len = RX_BUF_SIZE;
		wmb();

		i210_prepare_rx_desc(qidx, pdata, i);
	}

	err = i210_init_rx_desc(pdata, qidx);
	if (err != 0)
		return err;


	return 0;	
}

static int i210_alloc_rx_desc(struct i210_prv_data *pdata)
{
	struct rx_desc_wrap *wrx_desc_ptr = NULL;
	int i = 0, ret = 0;

	wrx_desc_ptr = kzalloc((RX_Q_CNT * sizeof(const struct rx_desc_wrap))
				  ,GFP_KERNEL);
	if (wrx_desc_ptr) {
		printk(KERN_ALERT "rx_wrapper desc failed\n");
		return -ENOMEM;
	}
	pdata->wrxdesc = wrx_desc_ptr;

	for (i = 0; i < RX_Q_CNT; i++) {
		ret = i210_alloc_rx_desc_oneq(pdata, i);
		if (ret != 0) {
			goto fail;
		}
	}

	return 0;
fail:
	kfree(wrx_desc_ptr);
	pdata->wrxdesc = NULL;

	return -ENOMEM;
}

static void i210_free_tx_desc(struct i210_prv_data *pdata)
{
	struct tx_desc_wrap *wtx_desc = NULL;
	int i;

	HANDLE_ME();
	for (i = 0; i < TX_Q_CNT; i++) {
		wtx_desc = &pdata->wtxdesc[i];
		dma_free_coherent(&pdata->pdev->dev, ((sizeof(struct tx_desc)) * TX_DESC_CNT),
							wtx_desc->txdesc[0], wtx_desc->txdesc_dma[0]);
		wtx_desc->txdesc[i] = NULL;
	}

	pdata->wtxdesc = NULL;
}

static void i210_free_rx_desc(struct i210_prv_data *pdata)
{
	struct rx_desc_wrap *wrx_desc = NULL;
	int i;

	HANDLE_ME();
	for (i = 0; i < RX_Q_CNT; i++) {
		wrx_desc = &pdata->wrxdesc[i];
		dma_free_coherent(&pdata->pdev->dev, ((sizeof(struct rx_desc)) * RX_DESC_CNT),
							wrx_desc->rxdesc[0], wrx_desc->rxdesc_dma[0]);
		wrx_desc->rxdesc[i] = NULL;
	}

	pdata->wrxdesc = NULL;
}

static int i210_alloc_desc(struct i210_prv_data *pdata)
{
	int err;

	err = i210_alloc_tx_desc(pdata);
	if (err != 0) {
		return err;
	}

	err = i210_alloc_rx_desc(pdata);
	if (err != 0) {
		i210_free_tx_desc(pdata);
		return err;
	}

	return 0;
}



static int i210_init_desc(struct i210_prv_data *pdata)
{
	int err;

	err = i210_alloc_desc(pdata);
	if (err != 0)
		return err;

	return 0;
}

static void i210_set_rx_mode(struct net_device *dev)
{
	struct i210_prv_data *pdata = netdev_priv(dev);
	int reg;


	if (dev->flags & IFF_PROMISC) {
		printk(KERN_ALERT "PROMISCUOUS MODE\n");
		reg = i210_readl(RCTL);
		reg |= (BIT(3) | BIT(4));
		i210_writel(reg, RCTL);
	} else if (dev->flags & IFF_ALLMULTI ||
		   netdev_mc_count(dev) > 1) {
		printk(KERN_ALERT "ALL MULTICAST MODE\n");
		HANDLE_ME();
	} else if (netdev_mc_count(dev)) {
		printk(KERN_ALERT "MULTICAST LIST MODE\n");
		reg = i210_readl(RCTL);
		reg |= (BIT(4));
		i210_writel(reg, RCTL);
		HANDLE_ME();
	} else if (netdev_mc_count(dev) == 0) {
		printk(KERN_ALERT "UNICAST MODE\n");
		reg = i210_readl(RCTL);
		reg |= (BIT(3));
		i210_writel(reg, RCTL);
	} else {
		printk(KERN_ALERT "No mode to configure\n");
	}
}

static int i210_open(struct net_device *netdev)
{
	struct i210_prv_data *pdata = netdev_priv(netdev);
	struct pci_dev *pdev = pdata->pdev;
	int ret, rah, ral, i, err = 0;
	char mac_addr[ETH_ALEN];

	//pm_runtime_get_sync(&pdev->dev);

	//netif_carrier_off(netdev);

	ret = pci_enable_msi(pdata->pdev);
	if (ret == 0) {
		pdata->interrupt_mode = MSI_INT;
	} else {
		printk(KERN_ALERT "Unable to allocate MSI interrupts, hence");
		printk(KERN_ALERT
		       "falling back to legacy interrupt mode: Error = %d\n",
		       ret);
		pdata->interrupt_mode = LEGACY_INT;
	}

	netdev->irq = pdev->irq;

	if (pdata->interrupt_mode == MSI_INT)
		ret = request_irq(pdev->irq, i210_isr_handler, 0, DEV_NAME,
				  pdata);
	else
		ret = request_irq(pdev->irq, i210_isr_handler, IRQF_SHARED,
				  DEV_NAME, pdata);

	if (ret) {
		printk(KERN_ALERT "failed to register the irq : %d\n",
		       pdev->irq);
		goto err_req_irq;
	}

	/* Read RAL and RAH */
	rah = i210_readl(RAH_0_15(0));
	ral = i210_readl(RAL_0_15(0));

	for (i = 0; i < ETH_ALEN; i++) {
		if (i < 4) {
			mac_addr[i] = (u8)(ral >> (i*8));
		} else {
			mac_addr[i] = (u8)(rah >> (i*8));
		}
	}

	memcpy(netdev->dev_addr, mac_addr, ETH_ALEN);

	printk(KERN_ALERT "mac addr : %#x:%#x:%#x:%#x:%#x:%#x\n",
	       netdev->dev_addr[0], netdev->dev_addr[1], netdev->dev_addr[2],
	       netdev->dev_addr[3], netdev->dev_addr[4], netdev->dev_addr[5]);


	i210_set_rx_mode(netdev);
	/* Start the DMA TX/RX */
	err = i210_init_desc(pdata);
	if (err != 0) {
		printk(KERN_ALERT "Failed to init the descriptor");
		return err;
	}

//	pm_runtime_put(&pdev->dev);
	/* Enable the MAC Rx/Tx */
	napi_enable(&pdata->napi);
	netif_tx_start_all_queues(netdev);

	return 0;

err_req_irq:
//	pm_runtime_put_sync(&pdev->dev);

	return err;
}

static int i210_close(struct net_device *netdev)
{
	struct i210_prv_data *pdata = netdev_priv(netdev);

	netif_tx_stop_all_queues(netdev);
	HANDLE_ME();
	//TODO:stop h/w

	napi_disable(&pdata->napi);
	i210_free_tx_desc(pdata);
	i210_free_rx_desc(pdata);

	if (pdata->irq_number != 0) {
		free_irq(pdata->irq_number, pdata);
		pci_disable_msi(pdata->pdev);
		pdata->irq_number = 0;
	}
	
	return 0;
}

static int i210_ioctl(struct net_device *netdev, struct ifreq *ifr, int cmd)
{
	HANDLE_ME();
	return 0;
}

static int i210_set_features(struct net_device *netdev,
			       netdev_features_t features)
{
	HANDLE_ME();
	return 0;
}

static void i210_poll_controller(struct net_device *netdev)
{
	struct i210_prv_data *pdata = netdev_priv(netdev);

	disable_irq(pdata->irq_number);
	i210_isr_handler(pdata->irq_number, pdata);
	enable_irq(pdata->irq_number);
}

static const struct net_device_ops i210_netdev_ops = {
	.ndo_open		= i210_open,
	.ndo_stop		= i210_close,
	.ndo_start_xmit		= i210_xmit,
	.ndo_set_rx_mode	= i210_set_rx_mode,
	//.ndo_set_mac_address	= i210_set_mac,
	//.ndo_change_mtu		= i210_change_mtu,
	.ndo_do_ioctl		= i210_ioctl,
	//.ndo_tx_timeout		= i210_tx_timeout,

#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= i210_poll_controller,
#endif
	.ndo_set_features = i210_set_features,
};

static void i210_hw_init(struct i210_prv_data *pdata)
{
	int reg;

	/* disable all intr */
	i210_writel((~0), IMC);

	/* SLU  */
	reg = i210_readl(CTRL);
	reg |= BIT(6);
	/* clear FRCSPD for MAC auto setting of PHY negotiation*/
	reg &= ~(BIT(11));
	i210_writel(reg, CTRL);
	i210_writel((~0), IMC);

	i210_writel(0, RCTL);
	i210_writel(BIT(3), TCTL);

	/* Program the TCTL register */
	reg = i210_readl(TCTL);
	/* TX enable and SWXOFF */
	reg |= (BIT(22) | BIT(1));
	i210_writel(reg, TCTL);

	/* Program the TXPBSIZE register */
	//TODO: Keeping it default i.e 20 kb

	/* Program the RXPBSIZE register  */
	//TODO: Keeping it default i.e 34 kb

#if 0
	/* Program RCTL with appropriate values. */
	reg = i210_readl(RCTL);
	/* RXEN */
	reg |= BIT(1);
	i210_writel(reg, RCTL);
#endif
	/* TXDW, RXDMT0, RX miss, RXDW ,Fatal error */
	reg = (BIT(0) | BIT(4) | BIT(6) | BIT(7) | BIT(22));
	i210_writel(reg, IMS);

	/* TXDW, RXDMT0, RX miss, RXDW ,Fatal error */
	reg = BIT(22);
	i210_writel(reg, ICS);
}


int i210_init(struct pci_dev *pdev, int pci_using_dac)
{
	struct net_device *netdev = NULL;
	struct i210_prv_data *pdata = NULL;
	int err = -ENOMEM;

	netdev = alloc_etherdev(sizeof(struct i210_prv_data));
	if (!netdev) {
		HANDLE_ERROR();
		goto err_alloc_etherdev;
	}	

	SET_NETDEV_DEV(netdev, &pdev->dev);

	netdev->irq = pdev->irq;

	pci_set_drvdata(pdev, netdev);
	pdata = netdev_priv(netdev);
	pdata->netdev = netdev;
	pdata->pdev = pdev;

	pdata->mem_start_addr = pci_resource_start(pdev, 0);
	pdata->mem_size = pci_resource_len(pdev, 0);

	err = -EIO;
	pdata->hw_addr = ioremap(pdata->mem_start_addr, pdata->mem_size);
	if (!pdata->hw_addr) {
		HANDLE_ERROR();
		goto err_ioremap;
	}

	/* construct the net_device struct */
	netdev->netdev_ops		= &i210_netdev_ops;
	/* TODO: i210_set_ethtool_ops(netdev); */

	netif_napi_add(netdev, &pdata->napi, i210_poll, 64);
	strlcpy(netdev->name, pci_name(pdev), sizeof(netdev->name));

	//FIXME: Need to add one by one feature later
#if 0
	/* Set initial default active device features */
	netdev->features = (NETIF_F_SG |
			    NETIF_F_HW_VLAN_RX |
			    NETIF_F_HW_VLAN_TX |
			    NETIF_F_TSO |
			    NETIF_F_TSO6 |
			    //NETIF_F_RXHASH |
			    NETIF_F_RXCSUM |
			    NETIF_F_HW_CSUM);

	/* Set user-changeable features (subset of all device features) */
	netdev->hw_features = netdev->features;
#endif
	if (pci_using_dac) {
		netdev->features |= NETIF_F_HIGHDMA;
		netdev->vlan_features |= NETIF_F_HIGHDMA;
	}

	/* Initialize the MAC Core */
	i210_hw_init(pdata);

	i210_init_mdio(netdev);

	err = register_netdev(netdev);

err_ioremap:
err_alloc_etherdev:
	return err;
}
