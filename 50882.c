static int timerslack_ns_open(struct inode *inode, struct file *filp)
{
	return single_open(filp, timerslack_ns_show, inode);
}
