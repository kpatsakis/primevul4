static void udf_write_failed(struct address_space *mapping, loff_t to)
{
	struct inode *inode = mapping->host;
	struct udf_inode_info *iinfo = UDF_I(inode);
	loff_t isize = inode->i_size;

	if (to > isize) {
		truncate_pagecache(inode, isize);
		if (iinfo->i_alloc_type != ICBTAG_FLAG_AD_IN_ICB) {
			down_write(&iinfo->i_data_sem);
			udf_clear_extent_cache(inode);
			udf_truncate_extents(inode);
			up_write(&iinfo->i_data_sem);
		}
	}
}
