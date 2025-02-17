static ssize_t tg3_show_temp(struct device *dev,
			     struct device_attribute *devattr, char *buf)
{
	struct pci_dev *pdev = to_pci_dev(dev);
	struct net_device *netdev = pci_get_drvdata(pdev);
	struct tg3 *tp = netdev_priv(netdev);
	struct sensor_device_attribute *attr = to_sensor_dev_attr(devattr);
	u32 temperature;

	spin_lock_bh(&tp->lock);
	tg3_ape_scratchpad_read(tp, &temperature, attr->index,
				sizeof(temperature));
	spin_unlock_bh(&tp->lock);
	return sprintf(buf, "%u\n", temperature);
}
