static void __net_exit xfrm_user_net_exit(struct list_head *net_exit_list)
{
	struct net *net;
	list_for_each_entry(net, net_exit_list, exit_list)
		RCU_INIT_POINTER(net->xfrm.nlsk, NULL);
	synchronize_net();
	list_for_each_entry(net, net_exit_list, exit_list)
		netlink_kernel_release(net->xfrm.nlsk_stash);
}
