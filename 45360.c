static int __pppoe_xmit(struct sock *sk, struct sk_buff *skb)
{
	struct pppox_sock *po = pppox_sk(sk);
	struct net_device *dev = po->pppoe_dev;
	struct pppoe_hdr *ph;
	int data_len = skb->len;

	/* The higher-level PPP code (ppp_unregister_channel()) ensures the PPP
	 * xmit operations conclude prior to an unregistration call.  Thus
	 * sk->sk_state cannot change, so we don't need to do lock_sock().
	 * But, we also can't do a lock_sock since that introduces a potential
	 * deadlock as we'd reverse the lock ordering used when calling
	 * ppp_unregister_channel().
	 */

	if (sock_flag(sk, SOCK_DEAD) || !(sk->sk_state & PPPOX_CONNECTED))
		goto abort;

	if (!dev)
		goto abort;

	/* Copy the data if there is no space for the header or if it's
	 * read-only.
	 */
	if (skb_cow_head(skb, sizeof(*ph) + dev->hard_header_len))
		goto abort;

	__skb_push(skb, sizeof(*ph));
	skb_reset_network_header(skb);

	ph = pppoe_hdr(skb);
	ph->ver	= 1;
	ph->type = 1;
	ph->code = 0;
	ph->sid	= po->num;
	ph->length = htons(data_len);

	skb->protocol = cpu_to_be16(ETH_P_PPP_SES);
	skb->dev = dev;

	dev_hard_header(skb, dev, ETH_P_PPP_SES,
			po->pppoe_pa.remote, NULL, data_len);

	dev_queue_xmit(skb);
	return 1;

abort:
	kfree_skb(skb);
	return 1;
}
