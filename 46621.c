void md_autodetect_dev(dev_t dev)
{
	struct detected_devices_node *node_detected_dev;

	node_detected_dev = kzalloc(sizeof(*node_detected_dev), GFP_KERNEL);
	if (node_detected_dev) {
		node_detected_dev->dev = dev;
		list_add_tail(&node_detected_dev->list, &all_detected_devices);
	} else {
		printk(KERN_CRIT "md: md_autodetect_dev: kzalloc failed"
			", skipping dev(%d,%d)\n", MAJOR(dev), MINOR(dev));
	}
}
