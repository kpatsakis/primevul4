static int udf_get_block(struct inode *inode, sector_t block,
			 struct buffer_head *bh_result, int create)
{
	int err, new;
	sector_t phys = 0;
	struct udf_inode_info *iinfo;

	if (!create) {
		phys = udf_block_map(inode, block);
		if (phys)
			map_bh(bh_result, inode->i_sb, phys);
		return 0;
	}

	err = -EIO;
	new = 0;
	iinfo = UDF_I(inode);

	down_write(&iinfo->i_data_sem);
	if (block == iinfo->i_next_alloc_block + 1) {
		iinfo->i_next_alloc_block++;
		iinfo->i_next_alloc_goal++;
	}

	udf_clear_extent_cache(inode);
	phys = inode_getblk(inode, block, &err, &new);
	if (!phys)
		goto abort;

	if (new)
		set_buffer_new(bh_result);
	map_bh(bh_result, inode->i_sb, phys);

abort:
	up_write(&iinfo->i_data_sem);
	return err;
}
