static int ax25_info_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &ax25_info_seqops);
}
