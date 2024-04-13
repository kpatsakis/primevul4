int dev_hard_start_xmit(struct sk_buff *skb, struct net_device *dev,
			struct netdev_queue *txq)
{
	const struct net_device_ops *ops = dev->netdev_ops;
	int rc = NETDEV_TX_OK;

	if (likely(!skb->next)) {
		int features;

		/*
		 * If device doesnt need skb->dst, release it right now while
		 * its hot in this cpu cache
		 */
		if (dev->priv_flags & IFF_XMIT_DST_RELEASE)
			skb_dst_drop(skb);

		if (!list_empty(&ptype_all))
			dev_queue_xmit_nit(skb, dev);

		skb_orphan_try(skb);

		features = netif_skb_features(skb);

		if (vlan_tx_tag_present(skb) &&
		    !(features & NETIF_F_HW_VLAN_TX)) {
			skb = __vlan_put_tag(skb, vlan_tx_tag_get(skb));
			if (unlikely(!skb))
				goto out;

			skb->vlan_tci = 0;
		}

		if (netif_needs_gso(skb, features)) {
			if (unlikely(dev_gso_segment(skb, features)))
				goto out_kfree_skb;
			if (skb->next)
				goto gso;
		} else {
			if (skb_needs_linearize(skb, features) &&
			    __skb_linearize(skb))
				goto out_kfree_skb;

			/* If packet is not checksummed and device does not
			 * support checksumming for this protocol, complete
			 * checksumming here.
			 */
			if (skb->ip_summed == CHECKSUM_PARTIAL) {
				skb_set_transport_header(skb,
					skb_checksum_start_offset(skb));
				if (!(features & NETIF_F_ALL_CSUM) &&
				     skb_checksum_help(skb))
					goto out_kfree_skb;
			}
		}

		rc = ops->ndo_start_xmit(skb, dev);
		trace_net_dev_xmit(skb, rc);
		if (rc == NETDEV_TX_OK)
			txq_trans_update(txq);
		return rc;
	}

gso:
	do {
		struct sk_buff *nskb = skb->next;

		skb->next = nskb->next;
		nskb->next = NULL;

		/*
		 * If device doesnt need nskb->dst, release it right now while
		 * its hot in this cpu cache
		 */
		if (dev->priv_flags & IFF_XMIT_DST_RELEASE)
			skb_dst_drop(nskb);

		rc = ops->ndo_start_xmit(nskb, dev);
		trace_net_dev_xmit(nskb, rc);
		if (unlikely(rc != NETDEV_TX_OK)) {
			if (rc & ~NETDEV_TX_MASK)
				goto out_kfree_gso_skb;
			nskb->next = skb->next;
			skb->next = nskb;
			return rc;
		}
		txq_trans_update(txq);
		if (unlikely(netif_tx_queue_stopped(txq) && skb->next))
			return NETDEV_TX_BUSY;
	} while (skb->next);

out_kfree_gso_skb:
	if (likely(skb->next == NULL))
		skb->destructor = DEV_GSO_CB(skb)->destructor;
out_kfree_skb:
	kfree_skb(skb);
out:
	return rc;
}
