static ssize_t rd_show_configfs_dev_params(struct se_device *dev, char *b)
{
	struct rd_dev *rd_dev = RD_DEV(dev);

	ssize_t bl = sprintf(b, "TCM RamDisk ID: %u  RamDisk Makeup: rd_mcp\n",
			rd_dev->rd_dev_id);
	bl += sprintf(b + bl, "        PAGES/PAGE_SIZE: %u*%lu"
			"  SG_table_count: %u  nullio: %d\n", rd_dev->rd_page_count,
			PAGE_SIZE, rd_dev->sg_table_count,
			!!(rd_dev->rd_flags & RDF_NULLIO));
	return bl;
}
