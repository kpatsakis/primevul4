static struct xfrm_state *xfrm_state_construct(struct net *net,
					       struct xfrm_usersa_info *p,
					       struct nlattr **attrs,
					       int *errp)
{
	struct xfrm_state *x = xfrm_state_alloc(net);
	int err = -ENOMEM;

	if (!x)
		goto error_no_put;

	copy_from_user_state(x, p);

	if (attrs[XFRMA_SA_EXTRA_FLAGS])
		x->props.extra_flags = nla_get_u32(attrs[XFRMA_SA_EXTRA_FLAGS]);

	if ((err = attach_aead(x, attrs[XFRMA_ALG_AEAD])))
		goto error;
	if ((err = attach_auth_trunc(&x->aalg, &x->props.aalgo,
				     attrs[XFRMA_ALG_AUTH_TRUNC])))
		goto error;
	if (!x->props.aalgo) {
		if ((err = attach_auth(&x->aalg, &x->props.aalgo,
				       attrs[XFRMA_ALG_AUTH])))
			goto error;
	}
	if ((err = attach_crypt(x, attrs[XFRMA_ALG_CRYPT])))
		goto error;
	if ((err = attach_one_algo(&x->calg, &x->props.calgo,
				   xfrm_calg_get_byname,
				   attrs[XFRMA_ALG_COMP])))
		goto error;

	if (attrs[XFRMA_ENCAP]) {
		x->encap = kmemdup(nla_data(attrs[XFRMA_ENCAP]),
				   sizeof(*x->encap), GFP_KERNEL);
		if (x->encap == NULL)
			goto error;
	}

	if (attrs[XFRMA_TFCPAD])
		x->tfcpad = nla_get_u32(attrs[XFRMA_TFCPAD]);

	if (attrs[XFRMA_COADDR]) {
		x->coaddr = kmemdup(nla_data(attrs[XFRMA_COADDR]),
				    sizeof(*x->coaddr), GFP_KERNEL);
		if (x->coaddr == NULL)
			goto error;
	}

	xfrm_mark_get(attrs, &x->mark);

	err = __xfrm_init_state(x, false);
	if (err)
		goto error;

	if (attrs[XFRMA_SEC_CTX]) {
		err = security_xfrm_state_alloc(x,
						nla_data(attrs[XFRMA_SEC_CTX]));
		if (err)
			goto error;
	}

	if ((err = xfrm_alloc_replay_state_esn(&x->replay_esn, &x->preplay_esn,
					       attrs[XFRMA_REPLAY_ESN_VAL])))
		goto error;

	x->km.seq = p->seq;
	x->replay_maxdiff = net->xfrm.sysctl_aevent_rseqth;
	/* sysctl_xfrm_aevent_etime is in 100ms units */
	x->replay_maxage = (net->xfrm.sysctl_aevent_etime*HZ)/XFRM_AE_ETH_M;

	if ((err = xfrm_init_replay(x)))
		goto error;

	/* override default values from above */
	xfrm_update_ae_params(x, attrs, 0);

	return x;

error:
	x->km.state = XFRM_STATE_DEAD;
	xfrm_state_put(x);
error_no_put:
	*errp = err;
	return NULL;
}
