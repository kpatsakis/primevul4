static int pppoe_disc_rcv(struct sk_buff *skb, struct net_device *dev,
			  struct packet_type *pt, struct net_device *orig_dev)

{
	struct pppoe_hdr *ph;
	struct pppox_sock *po;
	struct pppoe_net *pn;

	skb = skb_share_check(skb, GFP_ATOMIC);
	if (!skb)
		goto out;

	if (!pskb_may_pull(skb, sizeof(struct pppoe_hdr)))
		goto abort;

	ph = pppoe_hdr(skb);
	if (ph->code != PADT_CODE)
		goto abort;

	pn = pppoe_pernet(dev_net(dev));
	po = get_item(pn, ph->sid, eth_hdr(skb)->h_source, dev->ifindex);
	if (po) {
		struct sock *sk = sk_pppox(po);

		bh_lock_sock(sk);

		/* If the user has locked the socket, just ignore
		 * the packet.  With the way two rcv protocols hook into
		 * one socket family type, we cannot (easily) distinguish
		 * what kind of SKB it is during backlog rcv.
		 */
		if (sock_owned_by_user(sk) == 0) {
			/* We're no longer connect at the PPPOE layer,
			 * and must wait for ppp channel to disconnect us.
			 */
			sk->sk_state = PPPOX_ZOMBIE;
		}

		bh_unlock_sock(sk);
		sock_put(sk);
	}

abort:
	kfree_skb(skb);
out:
	return NET_RX_SUCCESS; /* Lies... :-) */
}
