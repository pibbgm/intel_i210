#include "i210.h"
#include "i210_reg.h"

//TODO:define
#define I210_VENDOR_ID 0x1234
#define I210_DEVICE_ID 0x1234


static int i210_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
	int err, pci_using_dac;

	err = pci_enable_device_mem(pdev);
	if (err)
		return err;

	pci_using_dac = 0;
	err = dma_set_mask(&pdev->dev, DMA_BIT_MASK(64));
	if (!err) {
		err = dma_set_coherent_mask(&pdev->dev, DMA_BIT_MASK(64));
		if (!err)
			pci_using_dac = 1;
	} else {
		err = dma_set_mask(&pdev->dev, DMA_BIT_MASK(32));
		if (err) {
			err = dma_set_coherent_mask(&pdev->dev,
						    DMA_BIT_MASK(32));
			if (err) {
				dev_err(&pdev->dev, "No usable DMA configuration, aborting\n");
				goto err_dma;
			}
		}
	}

	err = pci_request_selected_regions_exclusive(pdev,
	                                  pci_select_bars(pdev, IORESOURCE_MEM),
	                                  DRV_NAME);
	if (err)
		goto err_pci_reg;

	pci_set_master(pdev);
	/* PCI config space info */
	err = pci_save_state(pdev);
	if (err)
		goto err_alloc_etherdev;

	err = i210_init(pdev, pci_using_dac);
	if (err)
		goto err_register;

//	if (pci_dev_run_wake(pdev))
//		pm_runtime_put_noidle(&pdev->dev);

	return 0;

err_register:
err_alloc_etherdev:
	pci_release_selected_regions(pdev,
	                             pci_select_bars(pdev, IORESOURCE_MEM));
err_pci_reg:
err_dma:
	pci_disable_device(pdev);
	return err;
}

static void i210_remove(struct pci_dev *pdev)
{
	struct net_device *netdev = pci_get_drvdata(pdev);
	struct i210_prv_data *pdata = netdev_priv(netdev);

	if (!(netdev->flags & IFF_UP)) {
		//TODO:	power_down_phy;
	}

	unregister_netdev(netdev);

//	if (pci_dev_run_wake(pdev))
//		pm_runtime_get_noresume(&pdev->dev);

	iounmap(pdata->hw_addr);
	pci_release_selected_regions(pdev,
	                             pci_select_bars(pdev, IORESOURCE_MEM));

	free_netdev(netdev);

	pci_disable_device(pdev);
}

static struct pci_device_id i210_id_table = { PCI_DEVICE(I210_VENDOR_ID, I210_DEVICE_ID) };

static struct pci_driver i210_driver = {
	.name =         DRV_NAME,
	.id_table =     &i210_id_table,
	.probe =        i210_probe,
	.remove =       i210_remove,
	/*TODO: .driver.pm =	I210_PM_OPS, */
};

static int __init i210_init_module(void)
{
	return pci_register_driver(&i210_driver);
}

static void __exit i210_exit_module(void)
{
	pci_unregister_driver(&i210_driver);
}

module_init(i210_init_module);
module_exit(i210_exit_module);
MODULE_DESCRIPTION("Experimental ethernet driver for i210 NIC");
MODULE_AUTHOR("Praveen Bajantri <pibbgm@gmail.com>");
MODULE_LICENSE("GPL");
