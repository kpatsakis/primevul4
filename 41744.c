static int xfrm_add_pol_expire(struct sk_buff *skb, struct nlmsghdr *nlh,
		struct nlattr **attrs)
{
	struct net *net = sock_net(skb->sk);
	struct xfrm_policy *xp;
	struct xfrm_user_polexpire *up = nlmsg_data(nlh);
	struct xfrm_userpolicy_info *p = &up->pol;
	u8 type = XFRM_POLICY_TYPE_MAIN;
	int err = -ENOENT;
	struct xfrm_mark m;
	u32 mark = xfrm_mark_get(attrs, &m);

	err = copy_from_user_policy_type(&type, attrs);
	if (err)
		return err;

	err = verify_policy_dir(p->dir);
	if (err)
		return err;

	if (p->index)
		xp = xfrm_policy_byid(net, mark, type, p->dir, p->index, 0, &err);
	else {
		struct nlattr *rt = attrs[XFRMA_SEC_CTX];
		struct xfrm_sec_ctx *ctx;

		err = verify_sec_ctx_len(attrs);
		if (err)
			return err;

		ctx = NULL;
		if (rt) {
			struct xfrm_user_sec_ctx *uctx = nla_data(rt);

			err = security_xfrm_policy_alloc(&ctx, uctx);
			if (err)
				return err;
		}
		xp = xfrm_policy_bysel_ctx(net, mark, type, p->dir,
					   &p->sel, ctx, 0, &err);
		security_xfrm_policy_free(ctx);
	}
	if (xp == NULL)
		return -ENOENT;

	if (unlikely(xp->walk.dead))
		goto out;

	err = 0;
	if (up->hard) {
		uid_t loginuid = audit_get_loginuid(current);
		u32 sessionid = audit_get_sessionid(current);
		u32 sid;

		security_task_getsecid(current, &sid);
		xfrm_policy_delete(xp, p->dir);
		xfrm_audit_policy_delete(xp, 1, loginuid, sessionid, sid);

	} else {
		WARN(1, "Dont know what to do with soft policy expire\n");
	}
	km_policy_expired(xp, p->dir, up->hard, current->pid);

out:
	xfrm_pol_put(xp);
	return err;
}
