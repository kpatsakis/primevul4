void tty_default_fops(struct file_operations *fops)
{
	*fops = tty_fops;
}
