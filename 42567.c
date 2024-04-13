static int br_nf_pre_routing_finish_bridge(struct sk_buff *skb)
{
	struct nf_bridge_info *nf_bridge = skb->nf_bridge;
	struct dst_entry *dst;

	skb->dev = bridge_parent(skb->dev);
	if (!skb->dev)
		goto free_skb;
	dst = skb_dst(skb);
	if (dst->hh) {
		neigh_hh_bridge(dst->hh, skb);
		skb->dev = nf_bridge->physindev;
		return br_handle_frame_finish(skb);
	} else if (dst->neighbour) {
		/* the neighbour function below overwrites the complete
		 * MAC header, so we save the Ethernet source address and
		 * protocol number. */
		skb_copy_from_linear_data_offset(skb, -(ETH_HLEN-ETH_ALEN), skb->nf_bridge->data, ETH_HLEN-ETH_ALEN);
		/* tell br_dev_xmit to continue with forwarding */
		nf_bridge->mask |= BRNF_BRIDGED_DNAT;
		return dst->neighbour->output(skb);
	}
free_skb:
	kfree_skb(skb);
	return 0;
}
