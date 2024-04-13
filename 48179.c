int ecryptfs_encrypt_page(struct page *page)
{
	struct inode *ecryptfs_inode;
	struct ecryptfs_crypt_stat *crypt_stat;
	char *enc_extent_virt;
	struct page *enc_extent_page = NULL;
	loff_t extent_offset;
	loff_t lower_offset;
	int rc = 0;

	ecryptfs_inode = page->mapping->host;
	crypt_stat =
		&(ecryptfs_inode_to_private(ecryptfs_inode)->crypt_stat);
	BUG_ON(!(crypt_stat->flags & ECRYPTFS_ENCRYPTED));
	enc_extent_page = alloc_page(GFP_USER);
	if (!enc_extent_page) {
		rc = -ENOMEM;
		ecryptfs_printk(KERN_ERR, "Error allocating memory for "
				"encrypted extent\n");
		goto out;
	}

	for (extent_offset = 0;
	     extent_offset < (PAGE_CACHE_SIZE / crypt_stat->extent_size);
	     extent_offset++) {
		rc = crypt_extent(crypt_stat, enc_extent_page, page,
				  extent_offset, ENCRYPT);
		if (rc) {
			printk(KERN_ERR "%s: Error encrypting extent; "
			       "rc = [%d]\n", __func__, rc);
			goto out;
		}
	}

	lower_offset = lower_offset_for_page(crypt_stat, page);
	enc_extent_virt = kmap(enc_extent_page);
	rc = ecryptfs_write_lower(ecryptfs_inode, enc_extent_virt, lower_offset,
				  PAGE_CACHE_SIZE);
	kunmap(enc_extent_page);
	if (rc < 0) {
		ecryptfs_printk(KERN_ERR,
			"Error attempting to write lower page; rc = [%d]\n",
			rc);
		goto out;
	}
	rc = 0;
out:
	if (enc_extent_page) {
		__free_page(enc_extent_page);
	}
	return rc;
}
