static int irias_seq_open(struct inode *inode, struct file *file)
{
	IRDA_ASSERT( irias_objects != NULL, return -EINVAL;);

	return seq_open(file, &irias_seq_ops);
}
