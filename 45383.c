static int pppoe_seq_open(struct inode *inode, struct file *file)
{
	return seq_open_net(inode, file, &pppoe_seq_ops,
			sizeof(struct seq_net_private));
}
