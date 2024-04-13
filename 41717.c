static int build_sadinfo(struct sk_buff *skb, struct net *net,
			 u32 pid, u32 seq, u32 flags)
{
	struct xfrmk_sadinfo si;
	struct xfrmu_sadhinfo sh;
	struct nlmsghdr *nlh;
	int err;
	u32 *f;

	nlh = nlmsg_put(skb, pid, seq, XFRM_MSG_NEWSADINFO, sizeof(u32), 0);
	if (nlh == NULL) /* shouldn't really happen ... */
		return -EMSGSIZE;

	f = nlmsg_data(nlh);
	*f = flags;
	xfrm_sad_getinfo(net, &si);

	sh.sadhmcnt = si.sadhmcnt;
	sh.sadhcnt = si.sadhcnt;

	err = nla_put_u32(skb, XFRMA_SAD_CNT, si.sadcnt);
	if (!err)
		err = nla_put(skb, XFRMA_SAD_HINFO, sizeof(sh), &sh);
	if (err) {
		nlmsg_cancel(skb, nlh);
		return err;
	}

	return nlmsg_end(skb, nlh);
}
