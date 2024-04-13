static int __ap_scan_bus(struct device *dev, void *data)
{
	return to_ap_dev(dev)->qid == (ap_qid_t)(unsigned long) data;
}
