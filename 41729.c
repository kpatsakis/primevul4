static int copy_to_user_state_extra(struct xfrm_state *x,
				    struct xfrm_usersa_info *p,
				    struct sk_buff *skb)
{
	int ret = 0;

	copy_to_user_state(x, p);

	if (x->coaddr) {
		ret = nla_put(skb, XFRMA_COADDR, sizeof(*x->coaddr), x->coaddr);
		if (ret)
			goto out;
	}
	if (x->lastused) {
		ret = nla_put_u64(skb, XFRMA_LASTUSED, x->lastused);
		if (ret)
			goto out;
	}
	if (x->aead) {
		ret = nla_put(skb, XFRMA_ALG_AEAD, aead_len(x->aead), x->aead);
		if (ret)
			goto out;
	}
	if (x->aalg) {
		ret = copy_to_user_auth(x->aalg, skb);
		if (!ret)
			ret = nla_put(skb, XFRMA_ALG_AUTH_TRUNC,
				      xfrm_alg_auth_len(x->aalg), x->aalg);
		if (ret)
			goto out;
	}
	if (x->ealg) {
		ret = nla_put(skb, XFRMA_ALG_CRYPT, xfrm_alg_len(x->ealg), x->ealg);
		if (ret)
			goto out;
	}
	if (x->calg) {
		ret = nla_put(skb, XFRMA_ALG_COMP, sizeof(*(x->calg)), x->calg);
		if (ret)
			goto out;
	}
	if (x->encap) {
		ret = nla_put(skb, XFRMA_ENCAP, sizeof(*x->encap), x->encap);
		if (ret)
			goto out;
	}
	if (x->tfcpad) {
		ret = nla_put_u32(skb, XFRMA_TFCPAD, x->tfcpad);
		if (ret)
			goto out;
	}
	ret = xfrm_mark_put(skb, &x->mark);
	if (ret)
		goto out;
	if (x->replay_esn) {
		ret = nla_put(skb, XFRMA_REPLAY_ESN_VAL,
			      xfrm_replay_state_esn_len(x->replay_esn),
			      x->replay_esn);
		if (ret)
			goto out;
	}
	if (x->security)
		ret = copy_sec_ctx(x->security, skb);
out:
	return ret;
}
