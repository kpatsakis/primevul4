static int ip_vs_info_open(struct inode *inode, struct file *file)
{
	return seq_open_net(inode, file, &ip_vs_info_seq_ops,
			sizeof(struct ip_vs_iter));
}
