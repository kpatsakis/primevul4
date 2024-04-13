static int cg_release(const char *path, struct fuse_file_info *fi)
{
	struct file_info *f = (struct file_info *)fi->fh;

	do_release_file_info(f);
	return 0;
}
