static int udf_mknod(struct inode *dir, struct dentry *dentry, umode_t mode,
		     dev_t rdev)
{
	struct inode *inode;
	struct udf_fileident_bh fibh;
	struct fileIdentDesc cfi, *fi;
	int err;
	struct udf_inode_info *iinfo;

	if (!old_valid_dev(rdev))
		return -EINVAL;

	err = -EIO;
	inode = udf_new_inode(dir, mode, &err);
	if (!inode)
		goto out;

	iinfo = UDF_I(inode);
	init_special_inode(inode, mode, rdev);
	fi = udf_add_entry(dir, dentry, &fibh, &cfi, &err);
	if (!fi) {
		inode_dec_link_count(inode);
		iput(inode);
		return err;
	}
	cfi.icb.extLength = cpu_to_le32(inode->i_sb->s_blocksize);
	cfi.icb.extLocation = cpu_to_lelb(iinfo->i_location);
	*(__le32 *)((struct allocDescImpUse *)cfi.icb.impUse)->impUse =
		cpu_to_le32(iinfo->i_unique & 0x00000000FFFFFFFFUL);
	udf_write_fi(dir, &cfi, fi, &fibh, NULL, NULL);
	if (UDF_I(dir)->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB)
		mark_inode_dirty(dir);
	mark_inode_dirty(inode);

	if (fibh.sbh != fibh.ebh)
		brelse(fibh.ebh);
	brelse(fibh.sbh);
	d_instantiate(dentry, inode);
	err = 0;

out:
	return err;
}
