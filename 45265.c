static __net_init int raw_init_net(struct net *net)
{
	if (!proc_create("raw", S_IRUGO, net->proc_net, &raw_seq_fops))
		return -ENOMEM;

	return 0;
}
