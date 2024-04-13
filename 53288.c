static int tid_smaps_open(struct inode *inode, struct file *file)
{
	return do_maps_open(inode, file, &proc_tid_smaps_op);
}
