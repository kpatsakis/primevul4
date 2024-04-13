cifs_iovec_read(struct file *file, const struct iovec *iov,
		 unsigned long nr_segs, loff_t *poffset)
{
	ssize_t rc;
	size_t len, cur_len;
	ssize_t total_read = 0;
	loff_t offset = *poffset;
	unsigned int npages;
	struct cifs_sb_info *cifs_sb;
	struct cifs_tcon *tcon;
	struct cifsFileInfo *open_file;
	struct cifs_readdata *rdata, *tmp;
	struct list_head rdata_list;
	pid_t pid;

	if (!nr_segs)
		return 0;

	len = iov_length(iov, nr_segs);
	if (!len)
		return 0;

	INIT_LIST_HEAD(&rdata_list);
	cifs_sb = CIFS_SB(file->f_path.dentry->d_sb);
	open_file = file->private_data;
	tcon = tlink_tcon(open_file->tlink);

	if (!tcon->ses->server->ops->async_readv)
		return -ENOSYS;

	if (cifs_sb->mnt_cifs_flags & CIFS_MOUNT_RWPIDFORWARD)
		pid = open_file->pid;
	else
		pid = current->tgid;

	if ((file->f_flags & O_ACCMODE) == O_WRONLY)
		cifs_dbg(FYI, "attempting read on write only file instance\n");

	do {
		cur_len = min_t(const size_t, len - total_read, cifs_sb->rsize);
		npages = DIV_ROUND_UP(cur_len, PAGE_SIZE);

		/* allocate a readdata struct */
		rdata = cifs_readdata_alloc(npages,
					    cifs_uncached_readv_complete);
		if (!rdata) {
			rc = -ENOMEM;
			goto error;
		}

		rc = cifs_read_allocate_pages(rdata, npages);
		if (rc)
			goto error;

		rdata->cfile = cifsFileInfo_get(open_file);
		rdata->nr_pages = npages;
		rdata->offset = offset;
		rdata->bytes = cur_len;
		rdata->pid = pid;
		rdata->pagesz = PAGE_SIZE;
		rdata->read_into_pages = cifs_uncached_read_into_pages;

		rc = cifs_retry_async_readv(rdata);
error:
		if (rc) {
			kref_put(&rdata->refcount,
				 cifs_uncached_readdata_release);
			break;
		}

		list_add_tail(&rdata->list, &rdata_list);
		offset += cur_len;
		len -= cur_len;
	} while (len > 0);

	/* if at least one read request send succeeded, then reset rc */
	if (!list_empty(&rdata_list))
		rc = 0;

	/* the loop below should proceed in the order of increasing offsets */
restart_loop:
	list_for_each_entry_safe(rdata, tmp, &rdata_list, list) {
		if (!rc) {
			ssize_t copied;

			/* FIXME: freezable sleep too? */
			rc = wait_for_completion_killable(&rdata->done);
			if (rc)
				rc = -EINTR;
			else if (rdata->result)
				rc = rdata->result;
			else {
				rc = cifs_readdata_to_iov(rdata, iov,
							nr_segs, *poffset,
							&copied);
				total_read += copied;
			}

			/* resend call if it's a retryable error */
			if (rc == -EAGAIN) {
				rc = cifs_retry_async_readv(rdata);
				goto restart_loop;
			}
		}
		list_del_init(&rdata->list);
		kref_put(&rdata->refcount, cifs_uncached_readdata_release);
	}

	cifs_stats_bytes_read(tcon, total_read);
	*poffset += total_read;

	/* mask nodata case */
	if (rc == -ENODATA)
		rc = 0;

	return total_read ? total_read : rc;
}
