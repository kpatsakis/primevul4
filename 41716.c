static int build_report(struct sk_buff *skb, u8 proto,
			struct xfrm_selector *sel, xfrm_address_t *addr)
{
	struct xfrm_user_report *ur;
	struct nlmsghdr *nlh;

	nlh = nlmsg_put(skb, 0, 0, XFRM_MSG_REPORT, sizeof(*ur), 0);
	if (nlh == NULL)
		return -EMSGSIZE;

	ur = nlmsg_data(nlh);
	ur->proto = proto;
	memcpy(&ur->sel, sel, sizeof(ur->sel));

	if (addr) {
		int err = nla_put(skb, XFRMA_COADDR, sizeof(*addr), addr);
		if (err) {
			nlmsg_cancel(skb, nlh);
			return err;
		}
	}
	return nlmsg_end(skb, nlh);
}
