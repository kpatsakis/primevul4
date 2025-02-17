static int tg3_get_macaddr_sparc(struct tg3 *tp)
{
	struct net_device *dev = tp->dev;
	struct pci_dev *pdev = tp->pdev;
	struct device_node *dp = pci_device_to_OF_node(pdev);
	const unsigned char *addr;
	int len;

	addr = of_get_property(dp, "local-mac-address", &len);
	if (addr && len == 6) {
		memcpy(dev->dev_addr, addr, 6);
		return 0;
	}
	return -ENODEV;
}
