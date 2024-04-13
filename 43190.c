static int udf_sync_inode(struct inode *inode)
{
	return udf_update_inode(inode, 1);
}
