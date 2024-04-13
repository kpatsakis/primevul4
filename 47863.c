static int cg_releasedir(const char *path, struct fuse_file_info *fi)
{
	struct file_info *d = (struct file_info *)fi->fh;

	do_release_file_info(d);
	return 0;
}
