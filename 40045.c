void bt_procfs_cleanup(struct net *net, const char *name)
{
	remove_proc_entry(name, net->proc_net);
}
