int ecryptfs_decrypt_page(struct page *page)
{
	struct inode *ecryptfs_inode;
	struct ecryptfs_crypt_stat *crypt_stat;
	char *page_virt;
	unsigned long extent_offset;
	loff_t lower_offset;
	int rc = 0;

	ecryptfs_inode = page->mapping->host;
	crypt_stat =
		&(ecryptfs_inode_to_private(ecryptfs_inode)->crypt_stat);
	BUG_ON(!(crypt_stat->flags & ECRYPTFS_ENCRYPTED));

	lower_offset = lower_offset_for_page(crypt_stat, page);
	page_virt = kmap(page);
	rc = ecryptfs_read_lower(page_virt, lower_offset, PAGE_CACHE_SIZE,
				 ecryptfs_inode);
	kunmap(page);
	if (rc < 0) {
		ecryptfs_printk(KERN_ERR,
			"Error attempting to read lower page; rc = [%d]\n",
			rc);
		goto out;
	}

	for (extent_offset = 0;
	     extent_offset < (PAGE_CACHE_SIZE / crypt_stat->extent_size);
	     extent_offset++) {
		rc = crypt_extent(crypt_stat, page, page,
				  extent_offset, DECRYPT);
		if (rc) {
			printk(KERN_ERR "%s: Error encrypting extent; "
			       "rc = [%d]\n", __func__, rc);
			goto out;
		}
	}
out:
	return rc;
}
