static int bt_seq_open(struct inode *inode, struct file *file)
{
	struct bt_sock_list *sk_list;
	struct bt_seq_state *s;

	sk_list = PDE_DATA(inode);
	s = __seq_open_private(file, &bt_seq_ops,
			       sizeof(struct bt_seq_state));
	if (!s)
		return -ENOMEM;

	s->l = sk_list;
	return 0;
}
