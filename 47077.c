struct inode *__udf_iget(struct super_block *sb, struct kernel_lb_addr *ino,
			 bool hidden_inode)
{
	unsigned long block = udf_get_lb_pblock(sb, ino, 0);
	struct inode *inode = iget_locked(sb, block);
	int err;

	if (!inode)
		return ERR_PTR(-ENOMEM);

	if (!(inode->i_state & I_NEW))
		return inode;

	memcpy(&UDF_I(inode)->i_location, ino, sizeof(struct kernel_lb_addr));
	err = udf_read_inode(inode, hidden_inode);
	if (err < 0) {
		iget_failed(inode);
		return ERR_PTR(err);
	}
	unlock_new_inode(inode);

	return inode;
}
