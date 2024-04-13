static int xfrm_get_policy(struct sk_buff *skb, struct nlmsghdr *nlh,
		struct nlattr **attrs)
{
	struct net *net = sock_net(skb->sk);
	struct xfrm_policy *xp;
	struct xfrm_userpolicy_id *p;
	u8 type = XFRM_POLICY_TYPE_MAIN;
	int err;
	struct km_event c;
	int delete;
	struct xfrm_mark m;
	u32 mark = xfrm_mark_get(attrs, &m);

	p = nlmsg_data(nlh);
	delete = nlh->nlmsg_type == XFRM_MSG_DELPOLICY;

	err = copy_from_user_policy_type(&type, attrs);
	if (err)
		return err;

	err = verify_policy_dir(p->dir);
	if (err)
		return err;

	if (p->index)
		xp = xfrm_policy_byid(net, mark, type, p->dir, p->index, delete, &err);
	else {
		struct nlattr *rt = attrs[XFRMA_SEC_CTX];
		struct xfrm_sec_ctx *ctx;

		err = verify_sec_ctx_len(attrs);
		if (err)
			return err;

		ctx = NULL;
		if (rt) {
			struct xfrm_user_sec_ctx *uctx = nla_data(rt);

			err = security_xfrm_policy_alloc(&ctx, uctx, GFP_KERNEL);
			if (err)
				return err;
		}
		xp = xfrm_policy_bysel_ctx(net, mark, type, p->dir, &p->sel,
					   ctx, delete, &err);
		security_xfrm_policy_free(ctx);
	}
	if (xp == NULL)
		return -ENOENT;

	if (!delete) {
		struct sk_buff *resp_skb;

		resp_skb = xfrm_policy_netlink(skb, xp, p->dir, nlh->nlmsg_seq);
		if (IS_ERR(resp_skb)) {
			err = PTR_ERR(resp_skb);
		} else {
			err = nlmsg_unicast(net->xfrm.nlsk, resp_skb,
					    NETLINK_CB(skb).portid);
		}
	} else {
		xfrm_audit_policy_delete(xp, err ? 0 : 1, true);

		if (err != 0)
			goto out;

		c.data.byid = p->index;
		c.event = nlh->nlmsg_type;
		c.seq = nlh->nlmsg_seq;
		c.portid = nlh->nlmsg_pid;
		km_policy_notify(xp, p->dir, &c);
	}

out:
	xfrm_pol_put(xp);
	if (delete && err == 0)
		xfrm_garbage_collect(net);
	return err;
}
