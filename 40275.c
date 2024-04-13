static int nlmsg_populate_fdb_fill(struct sk_buff *skb,
				   struct net_device *dev,
				   u8 *addr, u32 pid, u32 seq,
				   int type, unsigned int flags)
{
	struct nlmsghdr *nlh;
	struct ndmsg *ndm;

	nlh = nlmsg_put(skb, pid, seq, type, sizeof(*ndm), NLM_F_MULTI);
	if (!nlh)
		return -EMSGSIZE;

	ndm = nlmsg_data(nlh);
	ndm->ndm_family  = AF_BRIDGE;
	ndm->ndm_pad1	 = 0;
	ndm->ndm_pad2    = 0;
	ndm->ndm_flags	 = flags;
	ndm->ndm_type	 = 0;
	ndm->ndm_ifindex = dev->ifindex;
	ndm->ndm_state   = NUD_PERMANENT;

	if (nla_put(skb, NDA_LLADDR, ETH_ALEN, addr))
		goto nla_put_failure;

	return nlmsg_end(skb, nlh);

nla_put_failure:
	nlmsg_cancel(skb, nlh);
	return -EMSGSIZE;
}
