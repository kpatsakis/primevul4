static int pfkey_spdadd(struct sock *sk, struct sk_buff *skb, const struct sadb_msg *hdr, void * const *ext_hdrs)
{
	struct net *net = sock_net(sk);
	int err = 0;
	struct sadb_lifetime *lifetime;
	struct sadb_address *sa;
	struct sadb_x_policy *pol;
	struct xfrm_policy *xp;
	struct km_event c;
	struct sadb_x_sec_ctx *sec_ctx;

	if (!present_and_same_family(ext_hdrs[SADB_EXT_ADDRESS_SRC-1],
				     ext_hdrs[SADB_EXT_ADDRESS_DST-1]) ||
	    !ext_hdrs[SADB_X_EXT_POLICY-1])
		return -EINVAL;

	pol = ext_hdrs[SADB_X_EXT_POLICY-1];
	if (pol->sadb_x_policy_type > IPSEC_POLICY_IPSEC)
		return -EINVAL;
	if (!pol->sadb_x_policy_dir || pol->sadb_x_policy_dir >= IPSEC_DIR_MAX)
		return -EINVAL;

	xp = xfrm_policy_alloc(net, GFP_KERNEL);
	if (xp == NULL)
		return -ENOBUFS;

	xp->action = (pol->sadb_x_policy_type == IPSEC_POLICY_DISCARD ?
		      XFRM_POLICY_BLOCK : XFRM_POLICY_ALLOW);
	xp->priority = pol->sadb_x_policy_priority;

	sa = ext_hdrs[SADB_EXT_ADDRESS_SRC-1],
	xp->family = pfkey_sadb_addr2xfrm_addr(sa, &xp->selector.saddr);
	if (!xp->family) {
		err = -EINVAL;
		goto out;
	}
	xp->selector.family = xp->family;
	xp->selector.prefixlen_s = sa->sadb_address_prefixlen;
	xp->selector.proto = pfkey_proto_to_xfrm(sa->sadb_address_proto);
	xp->selector.sport = ((struct sockaddr_in *)(sa+1))->sin_port;
	if (xp->selector.sport)
		xp->selector.sport_mask = htons(0xffff);

	sa = ext_hdrs[SADB_EXT_ADDRESS_DST-1],
	pfkey_sadb_addr2xfrm_addr(sa, &xp->selector.daddr);
	xp->selector.prefixlen_d = sa->sadb_address_prefixlen;

	/* Amusing, we set this twice.  KAME apps appear to set same value
	 * in both addresses.
	 */
	xp->selector.proto = pfkey_proto_to_xfrm(sa->sadb_address_proto);

	xp->selector.dport = ((struct sockaddr_in *)(sa+1))->sin_port;
	if (xp->selector.dport)
		xp->selector.dport_mask = htons(0xffff);

	sec_ctx = ext_hdrs[SADB_X_EXT_SEC_CTX - 1];
	if (sec_ctx != NULL) {
		struct xfrm_user_sec_ctx *uctx = pfkey_sadb2xfrm_user_sec_ctx(sec_ctx);

		if (!uctx) {
			err = -ENOBUFS;
			goto out;
		}

		err = security_xfrm_policy_alloc(&xp->security, uctx);
		kfree(uctx);

		if (err)
			goto out;
	}

	xp->lft.soft_byte_limit = XFRM_INF;
	xp->lft.hard_byte_limit = XFRM_INF;
	xp->lft.soft_packet_limit = XFRM_INF;
	xp->lft.hard_packet_limit = XFRM_INF;
	if ((lifetime = ext_hdrs[SADB_EXT_LIFETIME_HARD-1]) != NULL) {
		xp->lft.hard_packet_limit = _KEY2X(lifetime->sadb_lifetime_allocations);
		xp->lft.hard_byte_limit = _KEY2X(lifetime->sadb_lifetime_bytes);
		xp->lft.hard_add_expires_seconds = lifetime->sadb_lifetime_addtime;
		xp->lft.hard_use_expires_seconds = lifetime->sadb_lifetime_usetime;
	}
	if ((lifetime = ext_hdrs[SADB_EXT_LIFETIME_SOFT-1]) != NULL) {
		xp->lft.soft_packet_limit = _KEY2X(lifetime->sadb_lifetime_allocations);
		xp->lft.soft_byte_limit = _KEY2X(lifetime->sadb_lifetime_bytes);
		xp->lft.soft_add_expires_seconds = lifetime->sadb_lifetime_addtime;
		xp->lft.soft_use_expires_seconds = lifetime->sadb_lifetime_usetime;
	}
	xp->xfrm_nr = 0;
	if (pol->sadb_x_policy_type == IPSEC_POLICY_IPSEC &&
	    (err = parse_ipsecrequests(xp, pol)) < 0)
		goto out;

	err = xfrm_policy_insert(pol->sadb_x_policy_dir-1, xp,
				 hdr->sadb_msg_type != SADB_X_SPDUPDATE);

	xfrm_audit_policy_add(xp, err ? 0 : 1,
			      audit_get_loginuid(current),
			      audit_get_sessionid(current), 0);

	if (err)
		goto out;

	if (hdr->sadb_msg_type == SADB_X_SPDUPDATE)
		c.event = XFRM_MSG_UPDPOLICY;
	else
		c.event = XFRM_MSG_NEWPOLICY;

	c.seq = hdr->sadb_msg_seq;
	c.portid = hdr->sadb_msg_pid;

	km_policy_notify(xp, pol->sadb_x_policy_dir-1, &c);
	xfrm_pol_put(xp);
	return 0;

out:
	xp->walk.dead = 1;
	xfrm_policy_destroy(xp);
	return err;
}
