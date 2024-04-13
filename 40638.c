static int unicast_flush_resp(struct sock *sk, const struct sadb_msg *ihdr)
{
	struct sk_buff *skb;
	struct sadb_msg *hdr;

	skb = alloc_skb(sizeof(struct sadb_msg) + 16, GFP_ATOMIC);
	if (!skb)
		return -ENOBUFS;

	hdr = (struct sadb_msg *) skb_put(skb, sizeof(struct sadb_msg));
	memcpy(hdr, ihdr, sizeof(struct sadb_msg));
	hdr->sadb_msg_errno = (uint8_t) 0;
	hdr->sadb_msg_len = (sizeof(struct sadb_msg) / sizeof(uint64_t));

	return pfkey_broadcast(skb, GFP_ATOMIC, BROADCAST_ONE, sk, sock_net(sk));
}
