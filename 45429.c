int bt_procfs_init(struct net *net, const char *name,
		   struct bt_sock_list* sk_list,
		   int (* seq_show)(struct seq_file *, void *))
{
	sk_list->custom_seq_show = seq_show;

	if (!proc_create_data(name, 0, net->proc_net, &bt_fops, sk_list))
		return -ENOMEM;
	return 0;
}
