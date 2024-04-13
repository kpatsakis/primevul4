static struct sk_buff *compose_sadb_supported(const struct sadb_msg *orig,
					      gfp_t allocation)
{
	struct sk_buff *skb;
	struct sadb_msg *hdr;
	int len, auth_len, enc_len, i;

	auth_len = xfrm_count_pfkey_auth_supported();
	if (auth_len) {
		auth_len *= sizeof(struct sadb_alg);
		auth_len += sizeof(struct sadb_supported);
	}

	enc_len = xfrm_count_pfkey_enc_supported();
	if (enc_len) {
		enc_len *= sizeof(struct sadb_alg);
		enc_len += sizeof(struct sadb_supported);
	}

	len = enc_len + auth_len + sizeof(struct sadb_msg);

	skb = alloc_skb(len + 16, allocation);
	if (!skb)
		goto out_put_algs;

	hdr = (struct sadb_msg *) skb_put(skb, sizeof(*hdr));
	pfkey_hdr_dup(hdr, orig);
	hdr->sadb_msg_errno = 0;
	hdr->sadb_msg_len = len / sizeof(uint64_t);

	if (auth_len) {
		struct sadb_supported *sp;
		struct sadb_alg *ap;

		sp = (struct sadb_supported *) skb_put(skb, auth_len);
		ap = (struct sadb_alg *) (sp + 1);

		sp->sadb_supported_len = auth_len / sizeof(uint64_t);
		sp->sadb_supported_exttype = SADB_EXT_SUPPORTED_AUTH;

		for (i = 0; ; i++) {
			struct xfrm_algo_desc *aalg = xfrm_aalg_get_byidx(i);
			if (!aalg)
				break;
			if (!aalg->pfkey_supported)
				continue;
			if (aalg->available)
				*ap++ = aalg->desc;
		}
	}

	if (enc_len) {
		struct sadb_supported *sp;
		struct sadb_alg *ap;

		sp = (struct sadb_supported *) skb_put(skb, enc_len);
		ap = (struct sadb_alg *) (sp + 1);

		sp->sadb_supported_len = enc_len / sizeof(uint64_t);
		sp->sadb_supported_exttype = SADB_EXT_SUPPORTED_ENCRYPT;

		for (i = 0; ; i++) {
			struct xfrm_algo_desc *ealg = xfrm_ealg_get_byidx(i);
			if (!ealg)
				break;
			if (!ealg->pfkey_supported)
				continue;
			if (ealg->available)
				*ap++ = ealg->desc;
		}
	}

out_put_algs:
	return skb;
}
