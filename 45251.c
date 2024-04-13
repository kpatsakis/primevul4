static int dgram_disconnect(struct sock *sk, int flags)
{
	struct dgram_sock *ro = dgram_sk(sk);

	lock_sock(sk);

	ro->dst_addr.addr_type = IEEE802154_ADDR_LONG;
	memset(&ro->dst_addr.hwaddr, 0xff, sizeof(ro->dst_addr.hwaddr));

	release_sock(sk);

	return 0;
}
