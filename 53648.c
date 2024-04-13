static struct fuse_file *__fuse_write_file_get(struct fuse_conn *fc,
					       struct fuse_inode *fi)
{
	struct fuse_file *ff = NULL;

	spin_lock(&fc->lock);
	if (!list_empty(&fi->write_files)) {
		ff = list_entry(fi->write_files.next, struct fuse_file,
				write_entry);
		fuse_file_get(ff);
	}
	spin_unlock(&fc->lock);

	return ff;
}
