static struct dentry *udf_lookup(struct inode *dir, struct dentry *dentry,
				 struct nameidata *nd)
{
	struct inode *inode = NULL;
	struct fileIdentDesc cfi;
	struct udf_fileident_bh fibh;

	if (dentry->d_name.len > UDF_NAME_LEN - 2)
		return ERR_PTR(-ENAMETOOLONG);

#ifdef UDF_RECOVERY
	/* temporary shorthand for specifying files by inode number */
	if (!strncmp(dentry->d_name.name, ".B=", 3)) {
		struct kernel_lb_addr lb = {
			.logicalBlockNum = 0,
			.partitionReferenceNum =
				simple_strtoul(dentry->d_name.name + 3,
						NULL, 0),
		};
		inode = udf_iget(dir->i_sb, lb);
		if (!inode) {
			return ERR_PTR(-EACCES);
		}
	} else
#endif /* UDF_RECOVERY */

	if (udf_find_entry(dir, &dentry->d_name, &fibh, &cfi)) {
		struct kernel_lb_addr loc;

		if (fibh.sbh != fibh.ebh)
			brelse(fibh.ebh);
		brelse(fibh.sbh);

		loc = lelb_to_cpu(cfi.icb.extLocation);
		inode = udf_iget(dir->i_sb, &loc);
		if (!inode) {
			return ERR_PTR(-EACCES);
		}
	}

	return d_splice_alias(inode, dentry);
}
