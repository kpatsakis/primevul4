static int __net_init pfkey_init_proc(struct net *net)
{
	struct proc_dir_entry *e;

	e = proc_create("pfkey", 0, net->proc_net, &pfkey_proc_ops);
	if (e == NULL)
		return -ENOMEM;

	return 0;
}
