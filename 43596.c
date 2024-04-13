static int rd_configure_device(struct se_device *dev)
{
	struct rd_dev *rd_dev = RD_DEV(dev);
	struct rd_host *rd_host = dev->se_hba->hba_ptr;
	int ret;

	if (!(rd_dev->rd_flags & RDF_HAS_PAGE_COUNT)) {
		pr_debug("Missing rd_pages= parameter\n");
		return -EINVAL;
	}

	ret = rd_build_device_space(rd_dev);
	if (ret < 0)
		goto fail;

	dev->dev_attrib.hw_block_size = RD_BLOCKSIZE;
	dev->dev_attrib.hw_max_sectors = UINT_MAX;
	dev->dev_attrib.hw_queue_depth = RD_MAX_DEVICE_QUEUE_DEPTH;

	rd_dev->rd_dev_id = rd_host->rd_host_dev_id_count++;

	pr_debug("CORE_RD[%u] - Added TCM MEMCPY Ramdisk Device ID: %u of"
		" %u pages in %u tables, %lu total bytes\n",
		rd_host->rd_host_id, rd_dev->rd_dev_id, rd_dev->rd_page_count,
		rd_dev->sg_table_count,
		(unsigned long)(rd_dev->rd_page_count * PAGE_SIZE));

	return 0;

fail:
	rd_release_device_space(rd_dev);
	return ret;
}
