static int proc_tid_base_readdir(struct file *file, struct dir_context *ctx)
{
	return proc_pident_readdir(file, ctx,
				   tid_base_stuff, ARRAY_SIZE(tid_base_stuff));
}
