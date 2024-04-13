vhost_scsi_map_to_sgl(struct vhost_scsi_cmd *cmd,
		      void __user *ptr,
		      size_t len,
		      struct scatterlist *sgl,
		      bool write)
{
	unsigned int npages = 0, offset, nbytes;
	unsigned int pages_nr = iov_num_pages(ptr, len);
	struct scatterlist *sg = sgl;
	struct page **pages = cmd->tvc_upages;
	int ret, i;

	if (pages_nr > VHOST_SCSI_PREALLOC_UPAGES) {
		pr_err("vhost_scsi_map_to_sgl() pages_nr: %u greater than"
		       " preallocated VHOST_SCSI_PREALLOC_UPAGES: %u\n",
			pages_nr, VHOST_SCSI_PREALLOC_UPAGES);
		return -ENOBUFS;
	}

	ret = get_user_pages_fast((unsigned long)ptr, pages_nr, write, pages);
	/* No pages were pinned */
	if (ret < 0)
		goto out;
	/* Less pages pinned than wanted */
	if (ret != pages_nr) {
		for (i = 0; i < ret; i++)
			put_page(pages[i]);
		ret = -EFAULT;
		goto out;
	}

	while (len > 0) {
		offset = (uintptr_t)ptr & ~PAGE_MASK;
		nbytes = min_t(unsigned int, PAGE_SIZE - offset, len);
		sg_set_page(sg, pages[npages], nbytes, offset);
		ptr += nbytes;
		len -= nbytes;
		sg++;
		npages++;
	}

out:
	return ret;
}
