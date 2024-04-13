static int tid_numa_maps_open(struct inode *inode, struct file *file)
{
	return numa_maps_open(inode, file, &proc_tid_numa_maps_op);
}
