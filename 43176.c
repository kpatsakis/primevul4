int udf_expand_file_adinicb(struct inode *inode)
{
	struct page *page;
	char *kaddr;
	struct udf_inode_info *iinfo = UDF_I(inode);
	int err;
	struct writeback_control udf_wbc = {
		.sync_mode = WB_SYNC_NONE,
		.nr_to_write = 1,
	};

	WARN_ON_ONCE(!mutex_is_locked(&inode->i_mutex));
	if (!iinfo->i_lenAlloc) {
		if (UDF_QUERY_FLAG(inode->i_sb, UDF_FLAG_USE_SHORT_AD))
			iinfo->i_alloc_type = ICBTAG_FLAG_AD_SHORT;
		else
			iinfo->i_alloc_type = ICBTAG_FLAG_AD_LONG;
		/* from now on we have normal address_space methods */
		inode->i_data.a_ops = &udf_aops;
		up_write(&iinfo->i_data_sem);
		mark_inode_dirty(inode);
		return 0;
	}
	/*
	 * Release i_data_sem so that we can lock a page - page lock ranks
	 * above i_data_sem. i_mutex still protects us against file changes.
	 */
	up_write(&iinfo->i_data_sem);

	page = find_or_create_page(inode->i_mapping, 0, GFP_NOFS);
	if (!page)
		return -ENOMEM;

	if (!PageUptodate(page)) {
		kaddr = kmap(page);
		memset(kaddr + iinfo->i_lenAlloc, 0x00,
		       PAGE_CACHE_SIZE - iinfo->i_lenAlloc);
		memcpy(kaddr, iinfo->i_ext.i_data + iinfo->i_lenEAttr,
			iinfo->i_lenAlloc);
		flush_dcache_page(page);
		SetPageUptodate(page);
		kunmap(page);
	}
	down_write(&iinfo->i_data_sem);
	memset(iinfo->i_ext.i_data + iinfo->i_lenEAttr, 0x00,
	       iinfo->i_lenAlloc);
	iinfo->i_lenAlloc = 0;
	if (UDF_QUERY_FLAG(inode->i_sb, UDF_FLAG_USE_SHORT_AD))
		iinfo->i_alloc_type = ICBTAG_FLAG_AD_SHORT;
	else
		iinfo->i_alloc_type = ICBTAG_FLAG_AD_LONG;
	/* from now on we have normal address_space methods */
	inode->i_data.a_ops = &udf_aops;
	up_write(&iinfo->i_data_sem);
	err = inode->i_data.a_ops->writepage(page, &udf_wbc);
	if (err) {
		/* Restore everything back so that we don't lose data... */
		lock_page(page);
		kaddr = kmap(page);
		down_write(&iinfo->i_data_sem);
		memcpy(iinfo->i_ext.i_data + iinfo->i_lenEAttr, kaddr,
		       inode->i_size);
		kunmap(page);
		unlock_page(page);
		iinfo->i_alloc_type = ICBTAG_FLAG_AD_IN_ICB;
		inode->i_data.a_ops = &udf_adinicb_aops;
		up_write(&iinfo->i_data_sem);
	}
	page_cache_release(page);
	mark_inode_dirty(inode);

	return err;
}
