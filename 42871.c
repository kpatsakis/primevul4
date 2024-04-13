static int ipip_rcv(struct sk_buff *skb)
{
	struct ip_tunnel *tunnel;
	const struct iphdr *iph = ip_hdr(skb);

	rcu_read_lock();
	tunnel = ipip_tunnel_lookup(dev_net(skb->dev), iph->saddr, iph->daddr);
	if (tunnel != NULL) {
		struct pcpu_tstats *tstats;

		if (!xfrm4_policy_check(NULL, XFRM_POLICY_IN, skb)) {
			rcu_read_unlock();
			kfree_skb(skb);
			return 0;
		}

		secpath_reset(skb);

		skb->mac_header = skb->network_header;
		skb_reset_network_header(skb);
		skb->protocol = htons(ETH_P_IP);
		skb->pkt_type = PACKET_HOST;

		tstats = this_cpu_ptr(tunnel->dev->tstats);
		tstats->rx_packets++;
		tstats->rx_bytes += skb->len;

		__skb_tunnel_rx(skb, tunnel->dev);

		ipip_ecn_decapsulate(iph, skb);

		netif_rx(skb);

		rcu_read_unlock();
		return 0;
	}
	rcu_read_unlock();

	return -1;
}
