static int tid_maps_open(struct inode *inode, struct file *file)
{
	return do_maps_open(inode, file, &proc_tid_maps_op);
}
