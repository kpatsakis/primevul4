static int pfkey_migrate(struct sock *sk, struct sk_buff *skb,
			 const struct sadb_msg *hdr, void * const *ext_hdrs)
{
	return -ENOPROTOOPT;
}
