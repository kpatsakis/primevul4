static void fuse_aio_complete(struct fuse_io_priv *io, int err, ssize_t pos)
{
	bool is_sync = is_sync_kiocb(io->iocb);
	int left;

	spin_lock(&io->lock);
	if (err)
		io->err = io->err ? : err;
	else if (pos >= 0 && (io->bytes < 0 || pos < io->bytes))
		io->bytes = pos;

	left = --io->reqs;
	if (!left && is_sync)
		complete(io->done);
	spin_unlock(&io->lock);

	if (!left && !is_sync) {
		ssize_t res = fuse_get_res_by_io(io);

		if (res >= 0) {
			struct inode *inode = file_inode(io->iocb->ki_filp);
			struct fuse_conn *fc = get_fuse_conn(inode);
			struct fuse_inode *fi = get_fuse_inode(inode);

			spin_lock(&fc->lock);
			fi->attr_version = ++fc->attr_version;
			spin_unlock(&fc->lock);
		}

		io->iocb->ki_complete(io->iocb, res, 0);
		kfree(io);
	}
}
