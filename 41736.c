static int verify_newsa_info(struct xfrm_usersa_info *p,
			     struct nlattr **attrs)
{
	int err;

	err = -EINVAL;
	switch (p->family) {
	case AF_INET:
		break;

	case AF_INET6:
#if IS_ENABLED(CONFIG_IPV6)
		break;
#else
		err = -EAFNOSUPPORT;
		goto out;
#endif

	default:
		goto out;
	}

	err = -EINVAL;
	switch (p->id.proto) {
	case IPPROTO_AH:
		if ((!attrs[XFRMA_ALG_AUTH]	&&
		     !attrs[XFRMA_ALG_AUTH_TRUNC]) ||
		    attrs[XFRMA_ALG_AEAD]	||
		    attrs[XFRMA_ALG_CRYPT]	||
		    attrs[XFRMA_ALG_COMP]	||
		    attrs[XFRMA_TFCPAD])
			goto out;
		break;

	case IPPROTO_ESP:
		if (attrs[XFRMA_ALG_COMP])
			goto out;
		if (!attrs[XFRMA_ALG_AUTH] &&
		    !attrs[XFRMA_ALG_AUTH_TRUNC] &&
		    !attrs[XFRMA_ALG_CRYPT] &&
		    !attrs[XFRMA_ALG_AEAD])
			goto out;
		if ((attrs[XFRMA_ALG_AUTH] ||
		     attrs[XFRMA_ALG_AUTH_TRUNC] ||
		     attrs[XFRMA_ALG_CRYPT]) &&
		    attrs[XFRMA_ALG_AEAD])
			goto out;
		if (attrs[XFRMA_TFCPAD] &&
		    p->mode != XFRM_MODE_TUNNEL)
			goto out;
		break;

	case IPPROTO_COMP:
		if (!attrs[XFRMA_ALG_COMP]	||
		    attrs[XFRMA_ALG_AEAD]	||
		    attrs[XFRMA_ALG_AUTH]	||
		    attrs[XFRMA_ALG_AUTH_TRUNC]	||
		    attrs[XFRMA_ALG_CRYPT]	||
		    attrs[XFRMA_TFCPAD])
			goto out;
		break;

#if IS_ENABLED(CONFIG_IPV6)
	case IPPROTO_DSTOPTS:
	case IPPROTO_ROUTING:
		if (attrs[XFRMA_ALG_COMP]	||
		    attrs[XFRMA_ALG_AUTH]	||
		    attrs[XFRMA_ALG_AUTH_TRUNC]	||
		    attrs[XFRMA_ALG_AEAD]	||
		    attrs[XFRMA_ALG_CRYPT]	||
		    attrs[XFRMA_ENCAP]		||
		    attrs[XFRMA_SEC_CTX]	||
		    attrs[XFRMA_TFCPAD]		||
		    !attrs[XFRMA_COADDR])
			goto out;
		break;
#endif

	default:
		goto out;
	}

	if ((err = verify_aead(attrs)))
		goto out;
	if ((err = verify_auth_trunc(attrs)))
		goto out;
	if ((err = verify_one_alg(attrs, XFRMA_ALG_AUTH)))
		goto out;
	if ((err = verify_one_alg(attrs, XFRMA_ALG_CRYPT)))
		goto out;
	if ((err = verify_one_alg(attrs, XFRMA_ALG_COMP)))
		goto out;
	if ((err = verify_sec_ctx_len(attrs)))
		goto out;
	if ((err = verify_replay(p, attrs)))
		goto out;

	err = -EINVAL;
	switch (p->mode) {
	case XFRM_MODE_TRANSPORT:
	case XFRM_MODE_TUNNEL:
	case XFRM_MODE_ROUTEOPTIMIZATION:
	case XFRM_MODE_BEET:
		break;

	default:
		goto out;
	}

	err = 0;

out:
	return err;
}
