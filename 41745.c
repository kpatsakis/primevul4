static int xfrm_add_policy(struct sk_buff *skb, struct nlmsghdr *nlh,
		struct nlattr **attrs)
{
	struct net *net = sock_net(skb->sk);
	struct xfrm_userpolicy_info *p = nlmsg_data(nlh);
	struct xfrm_policy *xp;
	struct km_event c;
	int err;
	int excl;
	uid_t loginuid = audit_get_loginuid(current);
	u32 sessionid = audit_get_sessionid(current);
	u32 sid;

	err = verify_newpolicy_info(p);
	if (err)
		return err;
	err = verify_sec_ctx_len(attrs);
	if (err)
		return err;

	xp = xfrm_policy_construct(net, p, attrs, &err);
	if (!xp)
		return err;

	/* shouldn't excl be based on nlh flags??
	 * Aha! this is anti-netlink really i.e  more pfkey derived
	 * in netlink excl is a flag and you wouldnt need
	 * a type XFRM_MSG_UPDPOLICY - JHS */
	excl = nlh->nlmsg_type == XFRM_MSG_NEWPOLICY;
	err = xfrm_policy_insert(p->dir, xp, excl);
	security_task_getsecid(current, &sid);
	xfrm_audit_policy_add(xp, err ? 0 : 1, loginuid, sessionid, sid);

	if (err) {
		security_xfrm_policy_free(xp->security);
		kfree(xp);
		return err;
	}

	c.event = nlh->nlmsg_type;
	c.seq = nlh->nlmsg_seq;
	c.pid = nlh->nlmsg_pid;
	km_policy_notify(xp, p->dir, &c);

	xfrm_pol_put(xp);

	return 0;
}
