static int __net_init raw6_init_net(struct net *net)
{
	if (!proc_create("raw6", S_IRUGO, net->proc_net, &raw6_seq_fops))
		return -ENOMEM;

	return 0;
}
