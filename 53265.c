static int pid_numa_maps_open(struct inode *inode, struct file *file)
{
	return numa_maps_open(inode, file, &proc_pid_numa_maps_op);
}
