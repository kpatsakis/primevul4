static int numa_maps_open(struct inode *inode, struct file *file,
			  const struct seq_operations *ops)
{
	return proc_maps_open(inode, file, ops,
				sizeof(struct numa_maps_private));
}
