static netdev_tx_t ipip_tunnel_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct ip_tunnel *tunnel = netdev_priv(dev);
	struct pcpu_tstats *tstats;
	struct iphdr  *tiph = &tunnel->parms.iph;
	u8     tos = tunnel->parms.iph.tos;
	__be16 df = tiph->frag_off;
	struct rtable *rt;     			/* Route to the other host */
	struct net_device *tdev;		/* Device to other host */
	struct iphdr  *old_iph = ip_hdr(skb);
	struct iphdr  *iph;			/* Our new IP header */
	unsigned int max_headroom;		/* The extra header space needed */
	__be32 dst = tiph->daddr;
	int    mtu;

	if (skb->protocol != htons(ETH_P_IP))
		goto tx_error;

	if (tos & 1)
		tos = old_iph->tos;

	if (!dst) {
		/* NBMA tunnel */
		if ((rt = skb_rtable(skb)) == NULL) {
			dev->stats.tx_fifo_errors++;
			goto tx_error;
		}
		if ((dst = rt->rt_gateway) == 0)
			goto tx_error_icmp;
	}

	{
		struct flowi fl = {
			.oif = tunnel->parms.link,
			.fl4_dst = dst,
			.fl4_src= tiph->saddr,
			.fl4_tos = RT_TOS(tos),
			.proto = IPPROTO_IPIP
		};

		if (ip_route_output_key(dev_net(dev), &rt, &fl)) {
			dev->stats.tx_carrier_errors++;
			goto tx_error_icmp;
		}
	}
	tdev = rt->dst.dev;

	if (tdev == dev) {
		ip_rt_put(rt);
		dev->stats.collisions++;
		goto tx_error;
	}

	df |= old_iph->frag_off & htons(IP_DF);

	if (df) {
		mtu = dst_mtu(&rt->dst) - sizeof(struct iphdr);

		if (mtu < 68) {
			dev->stats.collisions++;
			ip_rt_put(rt);
			goto tx_error;
		}

		if (skb_dst(skb))
			skb_dst(skb)->ops->update_pmtu(skb_dst(skb), mtu);

		if ((old_iph->frag_off & htons(IP_DF)) &&
		    mtu < ntohs(old_iph->tot_len)) {
			icmp_send(skb, ICMP_DEST_UNREACH, ICMP_FRAG_NEEDED,
				  htonl(mtu));
			ip_rt_put(rt);
			goto tx_error;
		}
	}

	if (tunnel->err_count > 0) {
		if (time_before(jiffies,
				tunnel->err_time + IPTUNNEL_ERR_TIMEO)) {
			tunnel->err_count--;
			dst_link_failure(skb);
		} else
			tunnel->err_count = 0;
	}

	/*
	 * Okay, now see if we can stuff it in the buffer as-is.
	 */
	max_headroom = (LL_RESERVED_SPACE(tdev)+sizeof(struct iphdr));

	if (skb_headroom(skb) < max_headroom || skb_shared(skb) ||
	    (skb_cloned(skb) && !skb_clone_writable(skb, 0))) {
		struct sk_buff *new_skb = skb_realloc_headroom(skb, max_headroom);
		if (!new_skb) {
			ip_rt_put(rt);
			dev->stats.tx_dropped++;
			dev_kfree_skb(skb);
			return NETDEV_TX_OK;
		}
		if (skb->sk)
			skb_set_owner_w(new_skb, skb->sk);
		dev_kfree_skb(skb);
		skb = new_skb;
		old_iph = ip_hdr(skb);
	}

	skb->transport_header = skb->network_header;
	skb_push(skb, sizeof(struct iphdr));
	skb_reset_network_header(skb);
	memset(&(IPCB(skb)->opt), 0, sizeof(IPCB(skb)->opt));
	IPCB(skb)->flags &= ~(IPSKB_XFRM_TUNNEL_SIZE | IPSKB_XFRM_TRANSFORMED |
			      IPSKB_REROUTED);
	skb_dst_drop(skb);
	skb_dst_set(skb, &rt->dst);

	/*
	 *	Push down and install the IPIP header.
	 */

	iph 			=	ip_hdr(skb);
	iph->version		=	4;
	iph->ihl		=	sizeof(struct iphdr)>>2;
	iph->frag_off		=	df;
	iph->protocol		=	IPPROTO_IPIP;
	iph->tos		=	INET_ECN_encapsulate(tos, old_iph->tos);
	iph->daddr		=	rt->rt_dst;
	iph->saddr		=	rt->rt_src;

	if ((iph->ttl = tiph->ttl) == 0)
		iph->ttl	=	old_iph->ttl;

	nf_reset(skb);
	tstats = this_cpu_ptr(dev->tstats);
	__IPTUNNEL_XMIT(tstats, &dev->stats);
	return NETDEV_TX_OK;

tx_error_icmp:
	dst_link_failure(skb);
tx_error:
	dev->stats.tx_errors++;
	dev_kfree_skb(skb);
	return NETDEV_TX_OK;
}
