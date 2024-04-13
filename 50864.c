static int proc_projid_map_open(struct inode *inode, struct file *file)
{
	return proc_id_map_open(inode, file, &proc_projid_seq_operations);
}
