static int pfkey_send_migrate(const struct xfrm_selector *sel, u8 dir, u8 type,
			      const struct xfrm_migrate *m, int num_bundles,
			      const struct xfrm_kmaddress *k)
{
	int i;
	int sasize_sel;
	int size = 0;
	int size_pol = 0;
	struct sk_buff *skb;
	struct sadb_msg *hdr;
	struct sadb_x_policy *pol;
	const struct xfrm_migrate *mp;

	if (type != XFRM_POLICY_TYPE_MAIN)
		return 0;

	if (num_bundles <= 0 || num_bundles > XFRM_MAX_DEPTH)
		return -EINVAL;

	if (k != NULL) {
		/* addresses for KM */
		size += PFKEY_ALIGN8(sizeof(struct sadb_x_kmaddress) +
				     pfkey_sockaddr_pair_size(k->family));
	}

	/* selector */
	sasize_sel = pfkey_sockaddr_size(sel->family);
	if (!sasize_sel)
		return -EINVAL;
	size += (sizeof(struct sadb_address) + sasize_sel) * 2;

	/* policy info */
	size_pol += sizeof(struct sadb_x_policy);

	/* ipsecrequests */
	for (i = 0, mp = m; i < num_bundles; i++, mp++) {
		/* old locator pair */
		size_pol += sizeof(struct sadb_x_ipsecrequest) +
			    pfkey_sockaddr_pair_size(mp->old_family);
		/* new locator pair */
		size_pol += sizeof(struct sadb_x_ipsecrequest) +
			    pfkey_sockaddr_pair_size(mp->new_family);
	}

	size += sizeof(struct sadb_msg) + size_pol;

	/* alloc buffer */
	skb = alloc_skb(size, GFP_ATOMIC);
	if (skb == NULL)
		return -ENOMEM;

	hdr = (struct sadb_msg *)skb_put(skb, sizeof(struct sadb_msg));
	hdr->sadb_msg_version = PF_KEY_V2;
	hdr->sadb_msg_type = SADB_X_MIGRATE;
	hdr->sadb_msg_satype = pfkey_proto2satype(m->proto);
	hdr->sadb_msg_len = size / 8;
	hdr->sadb_msg_errno = 0;
	hdr->sadb_msg_reserved = 0;
	hdr->sadb_msg_seq = 0;
	hdr->sadb_msg_pid = 0;

	/* Addresses to be used by KM for negotiation, if ext is available */
	if (k != NULL && (set_sadb_kmaddress(skb, k) < 0))
		goto err;

	/* selector src */
	set_sadb_address(skb, sasize_sel, SADB_EXT_ADDRESS_SRC, sel);

	/* selector dst */
	set_sadb_address(skb, sasize_sel, SADB_EXT_ADDRESS_DST, sel);

	/* policy information */
	pol = (struct sadb_x_policy *)skb_put(skb, sizeof(struct sadb_x_policy));
	pol->sadb_x_policy_len = size_pol / 8;
	pol->sadb_x_policy_exttype = SADB_X_EXT_POLICY;
	pol->sadb_x_policy_type = IPSEC_POLICY_IPSEC;
	pol->sadb_x_policy_dir = dir + 1;
	pol->sadb_x_policy_id = 0;
	pol->sadb_x_policy_priority = 0;

	for (i = 0, mp = m; i < num_bundles; i++, mp++) {
		/* old ipsecrequest */
		int mode = pfkey_mode_from_xfrm(mp->mode);
		if (mode < 0)
			goto err;
		if (set_ipsecrequest(skb, mp->proto, mode,
				     (mp->reqid ?  IPSEC_LEVEL_UNIQUE : IPSEC_LEVEL_REQUIRE),
				     mp->reqid, mp->old_family,
				     &mp->old_saddr, &mp->old_daddr) < 0)
			goto err;

		/* new ipsecrequest */
		if (set_ipsecrequest(skb, mp->proto, mode,
				     (mp->reqid ? IPSEC_LEVEL_UNIQUE : IPSEC_LEVEL_REQUIRE),
				     mp->reqid, mp->new_family,
				     &mp->new_saddr, &mp->new_daddr) < 0)
			goto err;
	}

	/* broadcast migrate message to sockets */
	pfkey_broadcast(skb, GFP_ATOMIC, BROADCAST_ALL, NULL, &init_net);

	return 0;

err:
	kfree_skb(skb);
	return -EINVAL;
}
