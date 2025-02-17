static int ipxitf_demux_socket(struct ipx_interface *intrfc,
			       struct sk_buff *skb, int copy)
{
	struct ipxhdr *ipx = ipx_hdr(skb);
	struct sock *sock1 = NULL, *sock2 = NULL;
	struct sk_buff *skb1 = NULL, *skb2 = NULL;
	int rc;

	if (intrfc == ipx_primary_net && ntohs(ipx->ipx_dest.sock) == 0x451)
		sock1 = ncp_connection_hack(intrfc, ipx);
	if (!sock1)
		/* No special socket found, forward the packet the normal way */
		sock1 = ipxitf_find_socket(intrfc, ipx->ipx_dest.sock);

	/*
	 * We need to check if there is a primary net and if
	 * this is addressed to one of the *SPECIAL* sockets because
	 * these need to be propagated to the primary net.
	 * The *SPECIAL* socket list contains: 0x452(SAP), 0x453(RIP) and
	 * 0x456(Diagnostic).
	 */

	if (ipx_primary_net && intrfc != ipx_primary_net) {
		const int dsock = ntohs(ipx->ipx_dest.sock);

		if (dsock == 0x452 || dsock == 0x453 || dsock == 0x456)
			/* The appropriate thing to do here is to dup the
			 * packet and route to the primary net interface via
			 * ipxitf_send; however, we'll cheat and just demux it
			 * here. */
			sock2 = ipxitf_find_socket(ipx_primary_net,
							ipx->ipx_dest.sock);
	}

	/*
	 * If there is nothing to do return. The kfree will cancel any charging.
	 */
	rc = 0;
	if (!sock1 && !sock2) {
		if (!copy)
			kfree_skb(skb);
		goto out;
	}

	/*
	 * This next segment of code is a little awkward, but it sets it up
	 * so that the appropriate number of copies of the SKB are made and
	 * that skb1 and skb2 point to it (them) so that it (they) can be
	 * demuxed to sock1 and/or sock2.  If we are unable to make enough
	 * copies, we do as much as is possible.
	 */

	if (copy)
		skb1 = skb_clone(skb, GFP_ATOMIC);
	else
		skb1 = skb;

	rc = -ENOMEM;
	if (!skb1)
		goto out_put;

	/* Do we need 2 SKBs? */
	if (sock1 && sock2)
		skb2 = skb_clone(skb1, GFP_ATOMIC);
	else
		skb2 = skb1;

	if (sock1)
		ipxitf_def_skb_handler(sock1, skb1);

	if (!skb2)
		goto out_put;

	if (sock2)
		ipxitf_def_skb_handler(sock2, skb2);

	rc = 0;
out_put:
	if (sock1)
		sock_put(sock1);
	if (sock2)
		sock_put(sock2);
out:
	return rc;
}
