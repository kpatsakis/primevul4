static int udf_rename(struct inode *old_dir, struct dentry *old_dentry,
		      struct inode *new_dir, struct dentry *new_dentry)
{
	struct inode *old_inode = old_dentry->d_inode;
	struct inode *new_inode = new_dentry->d_inode;
	struct udf_fileident_bh ofibh, nfibh;
	struct fileIdentDesc *ofi = NULL, *nfi = NULL, *dir_fi = NULL;
	struct fileIdentDesc ocfi, ncfi;
	struct buffer_head *dir_bh = NULL;
	int retval = -ENOENT;
	struct kernel_lb_addr tloc;
	struct udf_inode_info *old_iinfo = UDF_I(old_inode);

	ofi = udf_find_entry(old_dir, &old_dentry->d_name, &ofibh, &ocfi);
	if (ofi) {
		if (ofibh.sbh != ofibh.ebh)
			brelse(ofibh.ebh);
		brelse(ofibh.sbh);
	}
	tloc = lelb_to_cpu(ocfi.icb.extLocation);
	if (!ofi || udf_get_lb_pblock(old_dir->i_sb, &tloc, 0)
	    != old_inode->i_ino)
		goto end_rename;

	nfi = udf_find_entry(new_dir, &new_dentry->d_name, &nfibh, &ncfi);
	if (nfi) {
		if (!new_inode) {
			if (nfibh.sbh != nfibh.ebh)
				brelse(nfibh.ebh);
			brelse(nfibh.sbh);
			nfi = NULL;
		}
	}
	if (S_ISDIR(old_inode->i_mode)) {
		int offset = udf_ext0_offset(old_inode);

		if (new_inode) {
			retval = -ENOTEMPTY;
			if (!empty_dir(new_inode))
				goto end_rename;
		}
		retval = -EIO;
		if (old_iinfo->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB) {
			dir_fi = udf_get_fileident(
					old_iinfo->i_ext.i_data -
					  (old_iinfo->i_efe ?
					   sizeof(struct extendedFileEntry) :
					   sizeof(struct fileEntry)),
					old_inode->i_sb->s_blocksize, &offset);
		} else {
			dir_bh = udf_bread(old_inode, 0, 0, &retval);
			if (!dir_bh)
				goto end_rename;
			dir_fi = udf_get_fileident(dir_bh->b_data,
					old_inode->i_sb->s_blocksize, &offset);
		}
		if (!dir_fi)
			goto end_rename;
		tloc = lelb_to_cpu(dir_fi->icb.extLocation);
		if (udf_get_lb_pblock(old_inode->i_sb, &tloc, 0) !=
				old_dir->i_ino)
			goto end_rename;
	}
	if (!nfi) {
		nfi = udf_add_entry(new_dir, new_dentry, &nfibh, &ncfi,
				    &retval);
		if (!nfi)
			goto end_rename;
	}

	/*
	 * Like most other Unix systems, set the ctime for inodes on a
	 * rename.
	 */
	old_inode->i_ctime = current_fs_time(old_inode->i_sb);
	mark_inode_dirty(old_inode);

	/*
	 * ok, that's it
	 */
	ncfi.fileVersionNum = ocfi.fileVersionNum;
	ncfi.fileCharacteristics = ocfi.fileCharacteristics;
	memcpy(&(ncfi.icb), &(ocfi.icb), sizeof(struct long_ad));
	udf_write_fi(new_dir, &ncfi, nfi, &nfibh, NULL, NULL);

	/* The old fid may have moved - find it again */
	ofi = udf_find_entry(old_dir, &old_dentry->d_name, &ofibh, &ocfi);
	udf_delete_entry(old_dir, ofi, &ofibh, &ocfi);

	if (new_inode) {
		new_inode->i_ctime = current_fs_time(new_inode->i_sb);
		inode_dec_link_count(new_inode);
	}
	old_dir->i_ctime = old_dir->i_mtime = current_fs_time(old_dir->i_sb);
	mark_inode_dirty(old_dir);

	if (dir_fi) {
		dir_fi->icb.extLocation = cpu_to_lelb(UDF_I(new_dir)->i_location);
		udf_update_tag((char *)dir_fi,
				(sizeof(struct fileIdentDesc) +
				le16_to_cpu(dir_fi->lengthOfImpUse) + 3) & ~3);
		if (old_iinfo->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB)
			mark_inode_dirty(old_inode);
		else
			mark_buffer_dirty_inode(dir_bh, old_inode);

		inode_dec_link_count(old_dir);
		if (new_inode)
			inode_dec_link_count(new_inode);
		else {
			inc_nlink(new_dir);
			mark_inode_dirty(new_dir);
		}
	}

	if (ofi) {
		if (ofibh.sbh != ofibh.ebh)
			brelse(ofibh.ebh);
		brelse(ofibh.sbh);
	}

	retval = 0;

end_rename:
	brelse(dir_bh);
	if (nfi) {
		if (nfibh.sbh != nfibh.ebh)
			brelse(nfibh.ebh);
		brelse(nfibh.sbh);
	}

	return retval;
}
