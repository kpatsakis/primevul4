static int dgram_init(struct sock *sk)
{
	struct dgram_sock *ro = dgram_sk(sk);

	ro->dst_addr.addr_type = IEEE802154_ADDR_LONG;
	ro->dst_addr.pan_id = 0xffff;
	ro->want_ack = 1;
	memset(&ro->dst_addr.hwaddr, 0xff, sizeof(ro->dst_addr.hwaddr));
	return 0;
}
