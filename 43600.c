static sector_t rd_get_blocks(struct se_device *dev)
{
	struct rd_dev *rd_dev = RD_DEV(dev);

	unsigned long long blocks_long = ((rd_dev->rd_page_count * PAGE_SIZE) /
			dev->dev_attrib.block_size) - 1;

	return blocks_long;
}
