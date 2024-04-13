static int crypt_extent(struct ecryptfs_crypt_stat *crypt_stat,
			struct page *dst_page,
			struct page *src_page,
			unsigned long extent_offset, int op)
{
	pgoff_t page_index = op == ENCRYPT ? src_page->index : dst_page->index;
	loff_t extent_base;
	char extent_iv[ECRYPTFS_MAX_IV_BYTES];
	struct scatterlist src_sg, dst_sg;
	size_t extent_size = crypt_stat->extent_size;
	int rc;

	extent_base = (((loff_t)page_index) * (PAGE_CACHE_SIZE / extent_size));
	rc = ecryptfs_derive_iv(extent_iv, crypt_stat,
				(extent_base + extent_offset));
	if (rc) {
		ecryptfs_printk(KERN_ERR, "Error attempting to derive IV for "
			"extent [0x%.16llx]; rc = [%d]\n",
			(unsigned long long)(extent_base + extent_offset), rc);
		goto out;
	}

	sg_init_table(&src_sg, 1);
	sg_init_table(&dst_sg, 1);

	sg_set_page(&src_sg, src_page, extent_size,
		    extent_offset * extent_size);
	sg_set_page(&dst_sg, dst_page, extent_size,
		    extent_offset * extent_size);

	rc = crypt_scatterlist(crypt_stat, &dst_sg, &src_sg, extent_size,
			       extent_iv, op);
	if (rc < 0) {
		printk(KERN_ERR "%s: Error attempting to crypt page with "
		       "page_index = [%ld], extent_offset = [%ld]; "
		       "rc = [%d]\n", __func__, page_index, extent_offset, rc);
		goto out;
	}
	rc = 0;
out:
	return rc;
}
