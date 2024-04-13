static int raw6_seq_open(struct inode *inode, struct file *file)
{
	return raw_seq_open(inode, file, &raw_v6_hashinfo, &raw6_seq_ops);
}
