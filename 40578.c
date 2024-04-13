static int pfkey_dump(struct sock *sk, struct sk_buff *skb, const struct sadb_msg *hdr, void * const *ext_hdrs)
{
	u8 proto;
	struct pfkey_sock *pfk = pfkey_sk(sk);

	if (pfk->dump.dump != NULL)
		return -EBUSY;

	proto = pfkey_satype2proto(hdr->sadb_msg_satype);
	if (proto == 0)
		return -EINVAL;

	pfk->dump.msg_version = hdr->sadb_msg_version;
	pfk->dump.msg_portid = hdr->sadb_msg_pid;
	pfk->dump.dump = pfkey_dump_sa;
	pfk->dump.done = pfkey_dump_sa_done;
	xfrm_state_walk_init(&pfk->dump.u.state, proto);

	return pfkey_do_dump(pfk);
}
