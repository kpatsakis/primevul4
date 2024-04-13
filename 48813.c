void kill_bdev(struct block_device *bdev)
{
	struct address_space *mapping = bdev->bd_inode->i_mapping;

	if (mapping->nrpages == 0 && mapping->nrshadows == 0)
		return;

	invalidate_bh_lrus();
	truncate_inode_pages(mapping, 0);
}	
