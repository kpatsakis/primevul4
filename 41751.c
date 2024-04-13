static int xfrm_del_sa(struct sk_buff *skb, struct nlmsghdr *nlh,
		struct nlattr **attrs)
{
	struct net *net = sock_net(skb->sk);
	struct xfrm_state *x;
	int err = -ESRCH;
	struct km_event c;
	struct xfrm_usersa_id *p = nlmsg_data(nlh);
	uid_t loginuid = audit_get_loginuid(current);
	u32 sessionid = audit_get_sessionid(current);
	u32 sid;

	x = xfrm_user_state_lookup(net, p, attrs, &err);
	if (x == NULL)
		return err;

	if ((err = security_xfrm_state_delete(x)) != 0)
		goto out;

	if (xfrm_state_kern(x)) {
		err = -EPERM;
		goto out;
	}

	err = xfrm_state_delete(x);

	if (err < 0)
		goto out;

	c.seq = nlh->nlmsg_seq;
	c.pid = nlh->nlmsg_pid;
	c.event = nlh->nlmsg_type;
	km_state_notify(x, &c);

out:
	security_task_getsecid(current, &sid);
	xfrm_audit_state_delete(x, err ? 0 : 1, loginuid, sessionid, sid);
	xfrm_state_put(x);
	return err;
}
