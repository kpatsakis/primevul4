static void sctp_v4_get_dst(struct sctp_transport *t, union sctp_addr *saddr,
				struct flowi *fl, struct sock *sk)
{
	struct sctp_association *asoc = t->asoc;
	struct rtable *rt;
	struct flowi4 *fl4 = &fl->u.ip4;
	struct sctp_bind_addr *bp;
	struct sctp_sockaddr_entry *laddr;
	struct dst_entry *dst = NULL;
	union sctp_addr *daddr = &t->ipaddr;
	union sctp_addr dst_saddr;

	memset(fl4, 0x0, sizeof(struct flowi4));
	fl4->daddr  = daddr->v4.sin_addr.s_addr;
	fl4->fl4_dport = daddr->v4.sin_port;
	fl4->flowi4_proto = IPPROTO_SCTP;
	if (asoc) {
		fl4->flowi4_tos = RT_CONN_FLAGS(asoc->base.sk);
		fl4->flowi4_oif = asoc->base.sk->sk_bound_dev_if;
		fl4->fl4_sport = htons(asoc->base.bind_addr.port);
	}
	if (saddr) {
		fl4->saddr = saddr->v4.sin_addr.s_addr;
		fl4->fl4_sport = saddr->v4.sin_port;
	}

	pr_debug("%s: dst:%pI4, src:%pI4 - ", __func__, &fl4->daddr,
		 &fl4->saddr);

	rt = ip_route_output_key(sock_net(sk), fl4);
	if (!IS_ERR(rt))
		dst = &rt->dst;

	/* If there is no association or if a source address is passed, no
	 * more validation is required.
	 */
	if (!asoc || saddr)
		goto out;

	bp = &asoc->base.bind_addr;

	if (dst) {
		/* Walk through the bind address list and look for a bind
		 * address that matches the source address of the returned dst.
		 */
		sctp_v4_dst_saddr(&dst_saddr, fl4, htons(bp->port));
		rcu_read_lock();
		list_for_each_entry_rcu(laddr, &bp->address_list, list) {
			if (!laddr->valid || (laddr->state == SCTP_ADDR_DEL) ||
			    (laddr->state != SCTP_ADDR_SRC &&
			    !asoc->src_out_of_asoc_ok))
				continue;
			if (sctp_v4_cmp_addr(&dst_saddr, &laddr->a))
				goto out_unlock;
		}
		rcu_read_unlock();

		/* None of the bound addresses match the source address of the
		 * dst. So release it.
		 */
		dst_release(dst);
		dst = NULL;
	}

	/* Walk through the bind address list and try to get a dst that
	 * matches a bind address as the source address.
	 */
	rcu_read_lock();
	list_for_each_entry_rcu(laddr, &bp->address_list, list) {
		struct net_device *odev;

		if (!laddr->valid)
			continue;
		if (laddr->state != SCTP_ADDR_SRC ||
		    AF_INET != laddr->a.sa.sa_family)
			continue;

		fl4->fl4_sport = laddr->a.v4.sin_port;
		flowi4_update_output(fl4,
				     asoc->base.sk->sk_bound_dev_if,
				     RT_CONN_FLAGS(asoc->base.sk),
				     daddr->v4.sin_addr.s_addr,
				     laddr->a.v4.sin_addr.s_addr);

		rt = ip_route_output_key(sock_net(sk), fl4);
		if (IS_ERR(rt))
			continue;

		if (!dst)
			dst = &rt->dst;

		/* Ensure the src address belongs to the output
		 * interface.
		 */
		odev = __ip_dev_find(sock_net(sk), laddr->a.v4.sin_addr.s_addr,
				     false);
		if (!odev || odev->ifindex != fl4->flowi4_oif) {
			if (&rt->dst != dst)
				dst_release(&rt->dst);
			continue;
		}

		if (dst != &rt->dst)
			dst_release(dst);
		dst = &rt->dst;
		break;
	}

out_unlock:
	rcu_read_unlock();
out:
	t->dst = dst;
	if (dst)
		pr_debug("rt_dst:%pI4, rt_src:%pI4\n",
			 &fl4->daddr, &fl4->saddr);
	else
		pr_debug("no route\n");
}
