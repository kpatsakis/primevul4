static __net_init int pppoe_init_net(struct net *net)
{
	struct pppoe_net *pn = pppoe_pernet(net);
	struct proc_dir_entry *pde;

	rwlock_init(&pn->hash_lock);

	pde = proc_create("pppoe", S_IRUGO, net->proc_net, &pppoe_seq_fops);
#ifdef CONFIG_PROC_FS
	if (!pde)
		return -ENOMEM;
#endif

	return 0;
}
