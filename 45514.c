static int ipxitf_demux_socket(struct ipx_interface *intrfc,
			       struct sk_buff *skb, int copy)
{
	struct ipxhdr *ipx = ipx_hdr(skb);
	int is_broadcast = !memcmp(ipx->ipx_dest.node, ipx_broadcast_node,
				   IPX_NODE_LEN);
	struct sock *s;
	int rc;

	spin_lock_bh(&intrfc->if_sklist_lock);

	sk_for_each(s, &intrfc->if_sklist) {
		struct ipx_sock *ipxs = ipx_sk(s);

		if (ipxs->port == ipx->ipx_dest.sock &&
		    (is_broadcast || !memcmp(ipx->ipx_dest.node,
					     ipxs->node, IPX_NODE_LEN))) {
			/* We found a socket to which to send */
			struct sk_buff *skb1;

			if (copy) {
				skb1 = skb_clone(skb, GFP_ATOMIC);
				rc = -ENOMEM;
				if (!skb1)
					goto out;
			} else {
				skb1 = skb;
				copy = 1; /* skb may only be used once */
			}
			ipxitf_def_skb_handler(s, skb1);

			/* On an external interface, one socket can listen */
			if (intrfc != ipx_internal_net)
				break;
		}
	}

	/* skb was solely for us, and we did not make a copy, so free it. */
	if (!copy)
		kfree_skb(skb);

	rc = 0;
out:
	spin_unlock_bh(&intrfc->if_sklist_lock);
	return rc;
}
