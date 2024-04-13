static int xfrm_flush_policy(struct sk_buff *skb, struct nlmsghdr *nlh,
		struct nlattr **attrs)
{
	struct net *net = sock_net(skb->sk);
	struct km_event c;
	u8 type = XFRM_POLICY_TYPE_MAIN;
	int err;
	struct xfrm_audit audit_info;

	err = copy_from_user_policy_type(&type, attrs);
	if (err)
		return err;

	audit_info.loginuid = audit_get_loginuid(current);
	audit_info.sessionid = audit_get_sessionid(current);
	security_task_getsecid(current, &audit_info.secid);
	err = xfrm_policy_flush(net, type, &audit_info);
	if (err) {
		if (err == -ESRCH) /* empty table */
			return 0;
		return err;
	}

	c.data.type = type;
	c.event = nlh->nlmsg_type;
	c.seq = nlh->nlmsg_seq;
	c.pid = nlh->nlmsg_pid;
	c.net = net;
	km_policy_notify(NULL, 0, &c);
	return 0;
}
