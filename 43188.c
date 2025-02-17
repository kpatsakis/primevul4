int udf_setsize(struct inode *inode, loff_t newsize)
{
	int err;
	struct udf_inode_info *iinfo;
	int bsize = 1 << inode->i_blkbits;

	if (!(S_ISREG(inode->i_mode) || S_ISDIR(inode->i_mode) ||
	      S_ISLNK(inode->i_mode)))
		return -EINVAL;
	if (IS_APPEND(inode) || IS_IMMUTABLE(inode))
		return -EPERM;

	iinfo = UDF_I(inode);
	if (newsize > inode->i_size) {
		down_write(&iinfo->i_data_sem);
		if (iinfo->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB) {
			if (bsize <
			    (udf_file_entry_alloc_offset(inode) + newsize)) {
				err = udf_expand_file_adinicb(inode);
				if (err)
					return err;
				down_write(&iinfo->i_data_sem);
			} else {
				iinfo->i_lenAlloc = newsize;
				goto set_size;
			}
		}
		err = udf_extend_file(inode, newsize);
		if (err) {
			up_write(&iinfo->i_data_sem);
			return err;
		}
set_size:
		truncate_setsize(inode, newsize);
		up_write(&iinfo->i_data_sem);
	} else {
		if (iinfo->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB) {
			down_write(&iinfo->i_data_sem);
			udf_clear_extent_cache(inode);
			memset(iinfo->i_ext.i_data + iinfo->i_lenEAttr + newsize,
			       0x00, bsize - newsize -
			       udf_file_entry_alloc_offset(inode));
			iinfo->i_lenAlloc = newsize;
			truncate_setsize(inode, newsize);
			up_write(&iinfo->i_data_sem);
			goto update_time;
		}
		err = block_truncate_page(inode->i_mapping, newsize,
					  udf_get_block);
		if (err)
			return err;
		down_write(&iinfo->i_data_sem);
		udf_clear_extent_cache(inode);
		truncate_setsize(inode, newsize);
		udf_truncate_extents(inode);
		up_write(&iinfo->i_data_sem);
	}
update_time:
	inode->i_mtime = inode->i_ctime = current_fs_time(inode->i_sb);
	if (IS_SYNC(inode))
		udf_sync_inode(inode);
	else
		mark_inode_dirty(inode);
 	return 0;
 }
