static struct xfrm_policy *xfrm_policy_construct(struct net *net, struct xfrm_userpolicy_info *p, struct nlattr **attrs, int *errp)
{
	struct xfrm_policy *xp = xfrm_policy_alloc(net, GFP_KERNEL);
	int err;

	if (!xp) {
		*errp = -ENOMEM;
		return NULL;
	}

	copy_from_user_policy(xp, p);

	err = copy_from_user_policy_type(&xp->type, attrs);
	if (err)
		goto error;

	if (!(err = copy_from_user_tmpl(xp, attrs)))
		err = copy_from_user_sec_ctx(xp, attrs);
	if (err)
		goto error;

	xfrm_mark_get(attrs, &xp->mark);

	return xp;
 error:
	*errp = err;
	xp->walk.dead = 1;
	xfrm_policy_destroy(xp);
	return NULL;
}
