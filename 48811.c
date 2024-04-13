void invalidate_bdev(struct block_device *bdev)
{
	struct address_space *mapping = bdev->bd_inode->i_mapping;

	if (mapping->nrpages == 0)
		return;

	invalidate_bh_lrus();
	lru_add_drain_all();	/* make sure all lru add caches are flushed */
	invalidate_mapping_pages(mapping, 0, -1);
	/* 99% of the time, we don't need to flush the cleancache on the bdev.
	 * But, for the strange corners, lets be cautious
	 */
	cleancache_invalidate_inode(mapping);
}
