ssize_t fuse_direct_io(struct fuse_io_priv *io, struct iov_iter *iter,
		       loff_t *ppos, int flags)
{
	int write = flags & FUSE_DIO_WRITE;
	int cuse = flags & FUSE_DIO_CUSE;
	struct file *file = io->file;
	struct inode *inode = file->f_mapping->host;
	struct fuse_file *ff = file->private_data;
	struct fuse_conn *fc = ff->fc;
	size_t nmax = write ? fc->max_write : fc->max_read;
	loff_t pos = *ppos;
	size_t count = iov_iter_count(iter);
	pgoff_t idx_from = pos >> PAGE_CACHE_SHIFT;
	pgoff_t idx_to = (pos + count - 1) >> PAGE_CACHE_SHIFT;
	ssize_t res = 0;
	struct fuse_req *req;

	if (io->async)
		req = fuse_get_req_for_background(fc, fuse_iter_npages(iter));
	else
		req = fuse_get_req(fc, fuse_iter_npages(iter));
	if (IS_ERR(req))
		return PTR_ERR(req);

	if (!cuse && fuse_range_is_writeback(inode, idx_from, idx_to)) {
		if (!write)
			mutex_lock(&inode->i_mutex);
		fuse_sync_writes(inode);
		if (!write)
			mutex_unlock(&inode->i_mutex);
	}

	while (count) {
		size_t nres;
		fl_owner_t owner = current->files;
		size_t nbytes = min(count, nmax);
		int err = fuse_get_user_pages(req, iter, &nbytes, write);
		if (err) {
			res = err;
			break;
		}

		if (write)
			nres = fuse_send_write(req, io, pos, nbytes, owner);
		else
			nres = fuse_send_read(req, io, pos, nbytes, owner);

		if (!io->async)
			fuse_release_user_pages(req, !write);
		if (req->out.h.error) {
			if (!res)
				res = req->out.h.error;
			break;
		} else if (nres > nbytes) {
			res = -EIO;
			break;
		}
		count -= nres;
		res += nres;
		pos += nres;
		if (nres != nbytes)
			break;
		if (count) {
			fuse_put_request(fc, req);
			if (io->async)
				req = fuse_get_req_for_background(fc,
					fuse_iter_npages(iter));
			else
				req = fuse_get_req(fc, fuse_iter_npages(iter));
			if (IS_ERR(req))
				break;
		}
	}
	if (!IS_ERR(req))
		fuse_put_request(fc, req);
	if (res > 0)
		*ppos = pos;

	return res;
}
