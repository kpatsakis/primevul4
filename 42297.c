static struct dentry *udf_get_parent(struct dentry *child)
{
	struct kernel_lb_addr tloc;
	struct inode *inode = NULL;
	struct qstr dotdot = QSTR_INIT("..", 2);
	struct fileIdentDesc cfi;
	struct udf_fileident_bh fibh;

	if (!udf_find_entry(child->d_inode, &dotdot, &fibh, &cfi))
		goto out_unlock;

	if (fibh.sbh != fibh.ebh)
		brelse(fibh.ebh);
	brelse(fibh.sbh);

	tloc = lelb_to_cpu(cfi.icb.extLocation);
	inode = udf_iget(child->d_inode->i_sb, &tloc);
	if (!inode)
		goto out_unlock;

	return d_obtain_alias(inode);
out_unlock:
	return ERR_PTR(-EACCES);
}
