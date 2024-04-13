static int dump_one_state(struct xfrm_state *x, int count, void *ptr)
{
	struct xfrm_dump_info *sp = ptr;
	struct sk_buff *in_skb = sp->in_skb;
	struct sk_buff *skb = sp->out_skb;
	struct xfrm_usersa_info *p;
	struct nlmsghdr *nlh;
	int err;

	nlh = nlmsg_put(skb, NETLINK_CB(in_skb).pid, sp->nlmsg_seq,
			XFRM_MSG_NEWSA, sizeof(*p), sp->nlmsg_flags);
	if (nlh == NULL)
		return -EMSGSIZE;

	p = nlmsg_data(nlh);

	err = copy_to_user_state_extra(x, p, skb);
	if (err) {
		nlmsg_cancel(skb, nlh);
		return err;
	}
	nlmsg_end(skb, nlh);
	return 0;
}
