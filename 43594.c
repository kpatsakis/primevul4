static inline struct rd_dev *RD_DEV(struct se_device *dev)
{
	return container_of(dev, struct rd_dev, dev);
}
