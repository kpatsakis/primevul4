static int xfrm_add_sa(struct sk_buff *skb, struct nlmsghdr *nlh,
		struct nlattr **attrs)
{
	struct net *net = sock_net(skb->sk);
	struct xfrm_usersa_info *p = nlmsg_data(nlh);
	struct xfrm_state *x;
	int err;
	struct km_event c;
	uid_t loginuid = audit_get_loginuid(current);
	u32 sessionid = audit_get_sessionid(current);
	u32 sid;

	err = verify_newsa_info(p, attrs);
	if (err)
		return err;

	x = xfrm_state_construct(net, p, attrs, &err);
	if (!x)
		return err;

	xfrm_state_hold(x);
	if (nlh->nlmsg_type == XFRM_MSG_NEWSA)
		err = xfrm_state_add(x);
	else
		err = xfrm_state_update(x);

	security_task_getsecid(current, &sid);
	xfrm_audit_state_add(x, err ? 0 : 1, loginuid, sessionid, sid);

	if (err < 0) {
		x->km.state = XFRM_STATE_DEAD;
		__xfrm_state_put(x);
		goto out;
	}

	c.seq = nlh->nlmsg_seq;
	c.pid = nlh->nlmsg_pid;
	c.event = nlh->nlmsg_type;

	km_state_notify(x, &c);
out:
	xfrm_state_put(x);
	return err;
}
