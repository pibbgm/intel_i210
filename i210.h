#include <linux/slab.h>
#include <linux/mm.h>
#include <asm/uaccess.h>
#include <linux/string.h>

#include <linux/pci.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/highmem.h>
#include <linux/proc_fs.h>
#include <linux/in.h>
#include <linux/ctype.h>
#include <linux/version.h>
#include <linux/ptrace.h>
#include <linux/dma-mapping.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/skbuff.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/crc32.h>
#include <linux/bitops.h>
#include <linux/mii.h>
#include <linux/phy.h>
#include <asm/processor.h>
#include <asm/dma.h>
#include <asm/page.h>
#include <asm/irq.h>
#include <net/checksum.h>
#include <linux/tcp.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/inet_lro.h>
#include <linux/semaphore.h>
/* for PTP */
#include <linux/net_tstamp.h>

#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>

/* Macro definitions*/

#include <asm-generic/errno.h>

#define DEV_NAME "i210"
#define DRV_NAME "i210"

#define TX_DESC_CNT 512
#define RX_DESC_CNT 512
#define TX_Q_CNT 1
#define RX_Q_CNT 1
#define RX_BUF_SIZE 2048
#define RX_DESC_MIN_CNT 16
#define MAX_PACKET_SIZE 1514

/* MII/GMII register offset */
#define AUTO_NEGO_NP 0x0007
#define PHY_CTL 0x0010
#define PHY_STS 0x0011

struct rx_desc {
	u64 buf_addr;
	u64 dw1;
};

struct tx_desc {
	u64 buf_addr;
	u64 dw1;
};

struct tx_buf {
	dma_addr_t buf_dma;
	struct sk_buff *skb;
	u32 len;
};

struct tx_desc_wrap {
	void *txdesc[TX_DESC_CNT];
	dma_addr_t txdesc_dma[TX_DESC_CNT];
	struct tx_buf txbuf[TX_DESC_CNT];
	u32 cur_tx;
	u32 durty_tx;
	u32 tx_queued;
	u32 tx_free_cnt;
};

struct rx_buf {
	dma_addr_t buf_dma;
	u32 len;
	struct sk_buff *skb;
};

struct rx_desc_wrap {
	void *rxdesc[RX_DESC_CNT];
	dma_addr_t rxdesc_dma[RX_DESC_CNT];
	struct rx_buf rxbuf[RX_DESC_CNT];
	u32 cur_rx;
	u32 dirty_rx;
	u32 rx_queued;
	u32 realloc_idx;
};

struct i210_phy_data {
	struct mii_bus *miibus;
	struct phy_device *phy_dev;
	int link_speed;
	int old_speed;
	int oldlink;
	int duplex;
	int oldduplex;
	int flow_control;
	int phy_addr;
	int bus_id;
};

struct i210_prv_data {
	struct net_device *netdev;
	struct pci_dev *pdev;
	struct napi_struct napi;
	
	spinlock_t rx_lock;
	spinlock_t tx_lock;

	unsigned int mem_start_addr;
	unsigned int mem_size;
	void __iomem *hw_addr;
	int irq_number;
	
	struct i210_phy_data phy_data;
	int tx_q_cnt;
	int rx_q_cnt;

	struct tx_desc_wrap *wtxdesc;
	struct rx_desc_wrap *wrxdesc;

#define LEGACY_INT 0
#define MSI_INT 1
	int interrupt_mode;
};

int i210_init(struct pci_dev *, int);

#define HANDLE_ERROR() printk(KERN_ALERT "HANDLE ERROR @ Line Number %d\n", __LINE__)
#define HANDLE_ME() printk(KERN_ALERT "HANDLE ME @ Line Number %d\n", __LINE__)
