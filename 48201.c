ecryptfs_write_metadata_to_contents(struct inode *ecryptfs_inode,
				    char *virt, size_t virt_len)
{
	int rc;

	rc = ecryptfs_write_lower(ecryptfs_inode, virt,
				  0, virt_len);
	if (rc < 0)
		printk(KERN_ERR "%s: Error attempting to write header "
		       "information to lower file; rc = [%d]\n", __func__, rc);
	else
		rc = 0;
	return rc;
}
