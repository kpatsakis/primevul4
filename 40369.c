static int proc_fb_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &proc_fb_seq_ops);
}
