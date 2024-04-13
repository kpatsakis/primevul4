void ndisc_send_redirect(struct sk_buff *skb, const struct in6_addr *target)
{
	struct net_device *dev = skb->dev;
	struct net *net = dev_net(dev);
	struct sock *sk = net->ipv6.ndisc_sk;
	int optlen = 0;
	struct inet_peer *peer;
	struct sk_buff *buff;
	struct rd_msg *msg;
	struct in6_addr saddr_buf;
	struct rt6_info *rt;
	struct dst_entry *dst;
	struct flowi6 fl6;
	int rd_len;
	u8 ha_buf[MAX_ADDR_LEN], *ha = NULL;
	bool ret;

	if (ipv6_get_lladdr(dev, &saddr_buf, IFA_F_TENTATIVE)) {
		ND_PRINTK(2, warn, "Redirect: no link-local address on %s\n",
			  dev->name);
		return;
	}

	if (!ipv6_addr_equal(&ipv6_hdr(skb)->daddr, target) &&
	    ipv6_addr_type(target) != (IPV6_ADDR_UNICAST|IPV6_ADDR_LINKLOCAL)) {
		ND_PRINTK(2, warn,
			  "Redirect: target address is not link-local unicast\n");
		return;
	}

	icmpv6_flow_init(sk, &fl6, NDISC_REDIRECT,
			 &saddr_buf, &ipv6_hdr(skb)->saddr, dev->ifindex);

	dst = ip6_route_output(net, NULL, &fl6);
	if (dst->error) {
		dst_release(dst);
		return;
	}
	dst = xfrm_lookup(net, dst, flowi6_to_flowi(&fl6), NULL, 0);
	if (IS_ERR(dst))
		return;

	rt = (struct rt6_info *) dst;

	if (rt->rt6i_flags & RTF_GATEWAY) {
		ND_PRINTK(2, warn,
			  "Redirect: destination is not a neighbour\n");
		goto release;
	}
	peer = inet_getpeer_v6(net->ipv6.peers, &rt->rt6i_dst.addr, 1);
	ret = inet_peer_xrlim_allow(peer, 1*HZ);
	if (peer)
		inet_putpeer(peer);
	if (!ret)
		goto release;

	if (dev->addr_len) {
		struct neighbour *neigh = dst_neigh_lookup(skb_dst(skb), target);
		if (!neigh) {
			ND_PRINTK(2, warn,
				  "Redirect: no neigh for target address\n");
			goto release;
		}

		read_lock_bh(&neigh->lock);
		if (neigh->nud_state & NUD_VALID) {
			memcpy(ha_buf, neigh->ha, dev->addr_len);
			read_unlock_bh(&neigh->lock);
			ha = ha_buf;
			optlen += ndisc_opt_addr_space(dev);
		} else
			read_unlock_bh(&neigh->lock);

		neigh_release(neigh);
	}

	rd_len = min_t(unsigned int,
		       IPV6_MIN_MTU - sizeof(struct ipv6hdr) - sizeof(*msg) - optlen,
		       skb->len + 8);
	rd_len &= ~0x7;
	optlen += rd_len;

	buff = ndisc_alloc_skb(dev, sizeof(*msg) + optlen);
	if (!buff)
		goto release;

	msg = (struct rd_msg *)skb_put(buff, sizeof(*msg));
	*msg = (struct rd_msg) {
		.icmph = {
			.icmp6_type = NDISC_REDIRECT,
		},
		.target = *target,
		.dest = ipv6_hdr(skb)->daddr,
	};

	/*
	 *	include target_address option
	 */

	if (ha)
		ndisc_fill_addr_option(buff, ND_OPT_TARGET_LL_ADDR, ha);

	/*
	 *	build redirect option and copy skb over to the new packet.
	 */

	if (rd_len)
		ndisc_fill_redirect_hdr_option(buff, skb, rd_len);

	skb_dst_set(buff, dst);
	ndisc_send_skb(buff, &ipv6_hdr(skb)->saddr, &saddr_buf);
	return;

release:
	dst_release(dst);
}
