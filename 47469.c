static void ndisc_recv_na(struct sk_buff *skb)
{
	struct nd_msg *msg = (struct nd_msg *)skb_transport_header(skb);
	struct in6_addr *saddr = &ipv6_hdr(skb)->saddr;
	const struct in6_addr *daddr = &ipv6_hdr(skb)->daddr;
	u8 *lladdr = NULL;
	u32 ndoptlen = skb_tail_pointer(skb) - (skb_transport_header(skb) +
				    offsetof(struct nd_msg, opt));
	struct ndisc_options ndopts;
	struct net_device *dev = skb->dev;
	struct inet6_ifaddr *ifp;
	struct neighbour *neigh;

	if (skb->len < sizeof(struct nd_msg)) {
		ND_PRINTK(2, warn, "NA: packet too short\n");
		return;
	}

	if (ipv6_addr_is_multicast(&msg->target)) {
		ND_PRINTK(2, warn, "NA: target address is multicast\n");
		return;
	}

	if (ipv6_addr_is_multicast(daddr) &&
	    msg->icmph.icmp6_solicited) {
		ND_PRINTK(2, warn, "NA: solicited NA is multicasted\n");
		return;
	}

	if (!ndisc_parse_options(msg->opt, ndoptlen, &ndopts)) {
		ND_PRINTK(2, warn, "NS: invalid ND option\n");
		return;
	}
	if (ndopts.nd_opts_tgt_lladdr) {
		lladdr = ndisc_opt_addr_data(ndopts.nd_opts_tgt_lladdr, dev);
		if (!lladdr) {
			ND_PRINTK(2, warn,
				  "NA: invalid link-layer address length\n");
			return;
		}
	}
	ifp = ipv6_get_ifaddr(dev_net(dev), &msg->target, dev, 1);
	if (ifp) {
		if (skb->pkt_type != PACKET_LOOPBACK
		    && (ifp->flags & IFA_F_TENTATIVE)) {
				addrconf_dad_failure(ifp);
				return;
		}
		/* What should we make now? The advertisement
		   is invalid, but ndisc specs say nothing
		   about it. It could be misconfiguration, or
		   an smart proxy agent tries to help us :-)

		   We should not print the error if NA has been
		   received from loopback - it is just our own
		   unsolicited advertisement.
		 */
		if (skb->pkt_type != PACKET_LOOPBACK)
			ND_PRINTK(1, warn,
				  "NA: someone advertises our address %pI6 on %s!\n",
				  &ifp->addr, ifp->idev->dev->name);
		in6_ifa_put(ifp);
		return;
	}
	neigh = neigh_lookup(&nd_tbl, &msg->target, dev);

	if (neigh) {
		u8 old_flags = neigh->flags;
		struct net *net = dev_net(dev);

		if (neigh->nud_state & NUD_FAILED)
			goto out;

		/*
		 * Don't update the neighbor cache entry on a proxy NA from
		 * ourselves because either the proxied node is off link or it
		 * has already sent a NA to us.
		 */
		if (lladdr && !memcmp(lladdr, dev->dev_addr, dev->addr_len) &&
		    net->ipv6.devconf_all->forwarding && net->ipv6.devconf_all->proxy_ndp &&
		    pneigh_lookup(&nd_tbl, net, &msg->target, dev, 0)) {
			/* XXX: idev->cnf.proxy_ndp */
			goto out;
		}

		neigh_update(neigh, lladdr,
			     msg->icmph.icmp6_solicited ? NUD_REACHABLE : NUD_STALE,
			     NEIGH_UPDATE_F_WEAK_OVERRIDE|
			     (msg->icmph.icmp6_override ? NEIGH_UPDATE_F_OVERRIDE : 0)|
			     NEIGH_UPDATE_F_OVERRIDE_ISROUTER|
			     (msg->icmph.icmp6_router ? NEIGH_UPDATE_F_ISROUTER : 0));

		if ((old_flags & ~neigh->flags) & NTF_ROUTER) {
			/*
			 * Change: router to host
			 */
			rt6_clean_tohost(dev_net(dev),  saddr);
		}

out:
		neigh_release(neigh);
	}
}
