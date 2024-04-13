static int pfkey_migrate(struct sock *sk, struct sk_buff *skb,
			 const struct sadb_msg *hdr, void * const *ext_hdrs)
{
	int i, len, ret, err = -EINVAL;
	u8 dir;
	struct sadb_address *sa;
	struct sadb_x_kmaddress *kma;
	struct sadb_x_policy *pol;
	struct sadb_x_ipsecrequest *rq;
	struct xfrm_selector sel;
	struct xfrm_migrate m[XFRM_MAX_DEPTH];
	struct xfrm_kmaddress k;

	if (!present_and_same_family(ext_hdrs[SADB_EXT_ADDRESS_SRC - 1],
				     ext_hdrs[SADB_EXT_ADDRESS_DST - 1]) ||
	    !ext_hdrs[SADB_X_EXT_POLICY - 1]) {
		err = -EINVAL;
		goto out;
	}

	kma = ext_hdrs[SADB_X_EXT_KMADDRESS - 1];
	pol = ext_hdrs[SADB_X_EXT_POLICY - 1];

	if (pol->sadb_x_policy_dir >= IPSEC_DIR_MAX) {
		err = -EINVAL;
		goto out;
	}

	if (kma) {
		/* convert sadb_x_kmaddress to xfrm_kmaddress */
		k.reserved = kma->sadb_x_kmaddress_reserved;
		ret = parse_sockaddr_pair((struct sockaddr *)(kma + 1),
					  8*(kma->sadb_x_kmaddress_len) - sizeof(*kma),
					  &k.local, &k.remote, &k.family);
		if (ret < 0) {
			err = ret;
			goto out;
		}
	}

	dir = pol->sadb_x_policy_dir - 1;
	memset(&sel, 0, sizeof(sel));

	/* set source address info of selector */
	sa = ext_hdrs[SADB_EXT_ADDRESS_SRC - 1];
	sel.family = pfkey_sadb_addr2xfrm_addr(sa, &sel.saddr);
	sel.prefixlen_s = sa->sadb_address_prefixlen;
	sel.proto = pfkey_proto_to_xfrm(sa->sadb_address_proto);
	sel.sport = ((struct sockaddr_in *)(sa + 1))->sin_port;
	if (sel.sport)
		sel.sport_mask = htons(0xffff);

	/* set destination address info of selector */
	sa = ext_hdrs[SADB_EXT_ADDRESS_DST - 1],
	pfkey_sadb_addr2xfrm_addr(sa, &sel.daddr);
	sel.prefixlen_d = sa->sadb_address_prefixlen;
	sel.proto = pfkey_proto_to_xfrm(sa->sadb_address_proto);
	sel.dport = ((struct sockaddr_in *)(sa + 1))->sin_port;
	if (sel.dport)
		sel.dport_mask = htons(0xffff);

	rq = (struct sadb_x_ipsecrequest *)(pol + 1);

	/* extract ipsecrequests */
	i = 0;
	len = pol->sadb_x_policy_len * 8 - sizeof(struct sadb_x_policy);

	while (len > 0 && i < XFRM_MAX_DEPTH) {
		ret = ipsecrequests_to_migrate(rq, len, &m[i]);
		if (ret < 0) {
			err = ret;
			goto out;
		} else {
			rq = (struct sadb_x_ipsecrequest *)((u8 *)rq + ret);
			len -= ret;
			i++;
		}
	}

	if (!i || len > 0) {
		err = -EINVAL;
		goto out;
	}

	return xfrm_migrate(&sel, dir, XFRM_POLICY_TYPE_MAIN, m, i,
			    kma ? &k : NULL);

 out:
	return err;
}
