static int ipip6_rcv(struct sk_buff *skb)
{
	struct iphdr *iph;
	struct ip_tunnel *tunnel;

	if (!pskb_may_pull(skb, sizeof(struct ipv6hdr)))
		goto out;

	iph = ip_hdr(skb);

	rcu_read_lock();
	tunnel = ipip6_tunnel_lookup(dev_net(skb->dev), skb->dev,
				     iph->saddr, iph->daddr);
	if (tunnel != NULL) {
		struct pcpu_tstats *tstats;

		secpath_reset(skb);
		skb->mac_header = skb->network_header;
		skb_reset_network_header(skb);
		IPCB(skb)->flags = 0;
		skb->protocol = htons(ETH_P_IPV6);
		skb->pkt_type = PACKET_HOST;

		if ((tunnel->dev->priv_flags & IFF_ISATAP) &&
		    !isatap_chksrc(skb, iph, tunnel)) {
			tunnel->dev->stats.rx_errors++;
			rcu_read_unlock();
			kfree_skb(skb);
			return 0;
		}

		tstats = this_cpu_ptr(tunnel->dev->tstats);
		tstats->rx_packets++;
		tstats->rx_bytes += skb->len;

		__skb_tunnel_rx(skb, tunnel->dev);

		ipip6_ecn_decapsulate(iph, skb);

		netif_rx(skb);

		rcu_read_unlock();
		return 0;
	}

	/* no tunnel matched,  let upstream know, ipsec may handle it */
	rcu_read_unlock();
	return 1;
out:
	kfree_skb(skb);
	return 0;
}
