static int xfrm_notify_sa_flush(const struct km_event *c)
{
	struct net *net = c->net;
	struct xfrm_usersa_flush *p;
	struct nlmsghdr *nlh;
	struct sk_buff *skb;
	int len = NLMSG_ALIGN(sizeof(struct xfrm_usersa_flush));

	skb = nlmsg_new(len, GFP_ATOMIC);
	if (skb == NULL)
		return -ENOMEM;

	nlh = nlmsg_put(skb, c->pid, c->seq, XFRM_MSG_FLUSHSA, sizeof(*p), 0);
	if (nlh == NULL) {
		kfree_skb(skb);
		return -EMSGSIZE;
	}

	p = nlmsg_data(nlh);
	p->proto = c->data.proto;

	nlmsg_end(skb, nlh);

	return nlmsg_multicast(net->xfrm.nlsk, skb, 0, XFRMNLGRP_SA, GFP_ATOMIC);
}
