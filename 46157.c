int btrfs_inode_check_errors(struct inode *inode)
{
	int ret = 0;

	if (test_bit(AS_ENOSPC, &inode->i_mapping->flags) &&
	    test_and_clear_bit(AS_ENOSPC, &inode->i_mapping->flags))
		ret = -ENOSPC;
	if (test_bit(AS_EIO, &inode->i_mapping->flags) &&
	    test_and_clear_bit(AS_EIO, &inode->i_mapping->flags))
		ret = -EIO;

	return ret;
}
