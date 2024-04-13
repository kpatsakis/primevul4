static int proc_attr_dir_readdir(struct file *file, struct dir_context *ctx)
{
	return proc_pident_readdir(file, ctx, 
				   attr_dir_stuff, ARRAY_SIZE(attr_dir_stuff));
}
