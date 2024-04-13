static void do_release_file_info(struct file_info *f)
{
	if (!f)
		return;
	free(f->controller);
	free(f->cgroup);
	free(f->file);
	free(f->buf);
	free(f);
}
