static int xfrm_add_acquire(struct sk_buff *skb, struct nlmsghdr *nlh,
		struct nlattr **attrs)
{
	struct net *net = sock_net(skb->sk);
	struct xfrm_policy *xp;
	struct xfrm_user_tmpl *ut;
	int i;
	struct nlattr *rt = attrs[XFRMA_TMPL];
	struct xfrm_mark mark;

	struct xfrm_user_acquire *ua = nlmsg_data(nlh);
	struct xfrm_state *x = xfrm_state_alloc(net);
	int err = -ENOMEM;

	if (!x)
		goto nomem;

	xfrm_mark_get(attrs, &mark);

	err = verify_newpolicy_info(&ua->policy);
	if (err)
		goto bad_policy;

	/*   build an XP */
	xp = xfrm_policy_construct(net, &ua->policy, attrs, &err);
	if (!xp)
		goto free_state;

	memcpy(&x->id, &ua->id, sizeof(ua->id));
	memcpy(&x->props.saddr, &ua->saddr, sizeof(ua->saddr));
	memcpy(&x->sel, &ua->sel, sizeof(ua->sel));
	xp->mark.m = x->mark.m = mark.m;
	xp->mark.v = x->mark.v = mark.v;
	ut = nla_data(rt);
	/* extract the templates and for each call km_key */
	for (i = 0; i < xp->xfrm_nr; i++, ut++) {
		struct xfrm_tmpl *t = &xp->xfrm_vec[i];
		memcpy(&x->id, &t->id, sizeof(x->id));
		x->props.mode = t->mode;
		x->props.reqid = t->reqid;
		x->props.family = ut->family;
		t->aalgos = ua->aalgos;
		t->ealgos = ua->ealgos;
		t->calgos = ua->calgos;
		err = km_query(x, t, xp);

	}

	kfree(x);
	kfree(xp);

	return 0;

bad_policy:
	WARN(1, "BAD policy passed\n");
free_state:
	kfree(x);
nomem:
	return err;
}
