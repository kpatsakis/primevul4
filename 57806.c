static void sctp_v6_get_dst(struct sctp_transport *t, union sctp_addr *saddr,
			    struct flowi *fl, struct sock *sk)
{
	struct sctp_association *asoc = t->asoc;
	struct dst_entry *dst = NULL;
	struct flowi6 *fl6 = &fl->u.ip6;
	struct sctp_bind_addr *bp;
	struct ipv6_pinfo *np = inet6_sk(sk);
	struct sctp_sockaddr_entry *laddr;
	union sctp_addr *daddr = &t->ipaddr;
	union sctp_addr dst_saddr;
	struct in6_addr *final_p, final;
	__u8 matchlen = 0;
	sctp_scope_t scope;

	memset(fl6, 0, sizeof(struct flowi6));
	fl6->daddr = daddr->v6.sin6_addr;
	fl6->fl6_dport = daddr->v6.sin6_port;
	fl6->flowi6_proto = IPPROTO_SCTP;
	if (ipv6_addr_type(&daddr->v6.sin6_addr) & IPV6_ADDR_LINKLOCAL)
		fl6->flowi6_oif = daddr->v6.sin6_scope_id;

	pr_debug("%s: dst=%pI6 ", __func__, &fl6->daddr);

	if (asoc)
		fl6->fl6_sport = htons(asoc->base.bind_addr.port);

	if (saddr) {
		fl6->saddr = saddr->v6.sin6_addr;
		fl6->fl6_sport = saddr->v6.sin6_port;

		pr_debug("src=%pI6 - ", &fl6->saddr);
	}

	rcu_read_lock();
	final_p = fl6_update_dst(fl6, rcu_dereference(np->opt), &final);
	rcu_read_unlock();

	dst = ip6_dst_lookup_flow(sk, fl6, final_p);
	if (!asoc || saddr)
		goto out;

	bp = &asoc->base.bind_addr;
	scope = sctp_scope(daddr);
	/* ip6_dst_lookup has filled in the fl6->saddr for us.  Check
	 * to see if we can use it.
	 */
	if (!IS_ERR(dst)) {
		/* Walk through the bind address list and look for a bind
		 * address that matches the source address of the returned dst.
		 */
		sctp_v6_to_addr(&dst_saddr, &fl6->saddr, htons(bp->port));
		rcu_read_lock();
		list_for_each_entry_rcu(laddr, &bp->address_list, list) {
			if (!laddr->valid || laddr->state == SCTP_ADDR_DEL ||
			    (laddr->state != SCTP_ADDR_SRC &&
			     !asoc->src_out_of_asoc_ok))
				continue;

			/* Do not compare against v4 addrs */
			if ((laddr->a.sa.sa_family == AF_INET6) &&
			    (sctp_v6_cmp_addr(&dst_saddr, &laddr->a))) {
				rcu_read_unlock();
				goto out;
			}
		}
		rcu_read_unlock();
		/* None of the bound addresses match the source address of the
		 * dst. So release it.
		 */
		dst_release(dst);
		dst = NULL;
	}

	/* Walk through the bind address list and try to get the
	 * best source address for a given destination.
	 */
	rcu_read_lock();
	list_for_each_entry_rcu(laddr, &bp->address_list, list) {
		struct dst_entry *bdst;
		__u8 bmatchlen;

		if (!laddr->valid ||
		    laddr->state != SCTP_ADDR_SRC ||
		    laddr->a.sa.sa_family != AF_INET6 ||
		    scope > sctp_scope(&laddr->a))
			continue;

		fl6->saddr = laddr->a.v6.sin6_addr;
		fl6->fl6_sport = laddr->a.v6.sin6_port;
		final_p = fl6_update_dst(fl6, rcu_dereference(np->opt), &final);
		bdst = ip6_dst_lookup_flow(sk, fl6, final_p);

		if (!IS_ERR(bdst) &&
		    ipv6_chk_addr(dev_net(bdst->dev),
				  &laddr->a.v6.sin6_addr, bdst->dev, 1)) {
			if (!IS_ERR_OR_NULL(dst))
				dst_release(dst);
			dst = bdst;
			break;
		}

		bmatchlen = sctp_v6_addr_match_len(daddr, &laddr->a);
		if (matchlen > bmatchlen)
			continue;

		if (!IS_ERR_OR_NULL(dst))
			dst_release(dst);
		dst = bdst;
		matchlen = bmatchlen;
	}
	rcu_read_unlock();

out:
	if (!IS_ERR_OR_NULL(dst)) {
		struct rt6_info *rt;

		rt = (struct rt6_info *)dst;
		t->dst = dst;
		t->dst_cookie = rt6_get_cookie(rt);
		pr_debug("rt6_dst:%pI6/%d rt6_src:%pI6\n",
			 &rt->rt6i_dst.addr, rt->rt6i_dst.plen,
			 &fl6->saddr);
	} else {
		t->dst = NULL;

		pr_debug("no route\n");
	}
}
