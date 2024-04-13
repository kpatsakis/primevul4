static int xfrm_get_ae(struct sk_buff *skb, struct nlmsghdr *nlh,
		struct nlattr **attrs)
{
	struct net *net = sock_net(skb->sk);
	struct xfrm_state *x;
	struct sk_buff *r_skb;
	int err;
	struct km_event c;
	u32 mark;
	struct xfrm_mark m;
	struct xfrm_aevent_id *p = nlmsg_data(nlh);
	struct xfrm_usersa_id *id = &p->sa_id;

	mark = xfrm_mark_get(attrs, &m);

	x = xfrm_state_lookup(net, mark, &id->daddr, id->spi, id->proto, id->family);
	if (x == NULL)
		return -ESRCH;

	r_skb = nlmsg_new(xfrm_aevent_msgsize(x), GFP_ATOMIC);
	if (r_skb == NULL) {
		xfrm_state_put(x);
		return -ENOMEM;
	}

	/*
	 * XXX: is this lock really needed - none of the other
	 * gets lock (the concern is things getting updated
	 * while we are still reading) - jhs
	*/
	spin_lock_bh(&x->lock);
	c.data.aevent = p->flags;
	c.seq = nlh->nlmsg_seq;
	c.pid = nlh->nlmsg_pid;

	if (build_aevent(r_skb, x, &c) < 0)
		BUG();
	err = nlmsg_unicast(net->xfrm.nlsk, r_skb, NETLINK_CB(skb).pid);
	spin_unlock_bh(&x->lock);
	xfrm_state_put(x);
	return err;
}
