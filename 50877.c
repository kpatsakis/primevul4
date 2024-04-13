static int proc_uid_map_open(struct inode *inode, struct file *file)
{
	return proc_id_map_open(inode, file, &proc_uid_seq_operations);
}
