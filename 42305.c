static int udf_rmdir(struct inode *dir, struct dentry *dentry)
{
	int retval;
	struct inode *inode = dentry->d_inode;
	struct udf_fileident_bh fibh;
	struct fileIdentDesc *fi, cfi;
	struct kernel_lb_addr tloc;

	retval = -ENOENT;
	fi = udf_find_entry(dir, &dentry->d_name, &fibh, &cfi);
	if (!fi)
		goto out;

	retval = -EIO;
	tloc = lelb_to_cpu(cfi.icb.extLocation);
	if (udf_get_lb_pblock(dir->i_sb, &tloc, 0) != inode->i_ino)
		goto end_rmdir;
	retval = -ENOTEMPTY;
	if (!empty_dir(inode))
		goto end_rmdir;
	retval = udf_delete_entry(dir, fi, &fibh, &cfi);
	if (retval)
		goto end_rmdir;
	if (inode->i_nlink != 2)
		udf_warn(inode->i_sb, "empty directory has nlink != 2 (%d)\n",
			 inode->i_nlink);
	clear_nlink(inode);
	inode->i_size = 0;
	inode_dec_link_count(dir);
	inode->i_ctime = dir->i_ctime = dir->i_mtime =
						current_fs_time(dir->i_sb);
	mark_inode_dirty(dir);

end_rmdir:
	if (fibh.sbh != fibh.ebh)
		brelse(fibh.ebh);
	brelse(fibh.sbh);

out:
	return retval;
}
