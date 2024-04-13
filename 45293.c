bool raw6_local_deliver(struct sk_buff *skb, int nexthdr)
{
	struct sock *raw_sk;

	raw_sk = sk_head(&raw_v6_hashinfo.ht[nexthdr & (RAW_HTABLE_SIZE - 1)]);
	if (raw_sk && !ipv6_raw_deliver(skb, nexthdr))
		raw_sk = NULL;

	return raw_sk != NULL;
}
