static void __net_exit tcpv6_net_exit_batch(struct list_head *net_exit_list)
{
	inet_twsk_purge(&tcp_hashinfo, AF_INET6);
}
