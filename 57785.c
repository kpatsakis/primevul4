static void __net_exit dccp_v6_exit_batch(struct list_head *net_exit_list)
{
	inet_twsk_purge(&dccp_hashinfo, AF_INET6);
}
