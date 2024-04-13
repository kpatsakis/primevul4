bool batadv_frag_skb_buffer(struct sk_buff **skb,
			    struct batadv_orig_node *orig_node_src)
{
	struct sk_buff *skb_out = NULL;
	struct hlist_head head = HLIST_HEAD_INIT;
	bool ret = false;

	/* Add packet to buffer and table entry if merge is possible. */
	if (!batadv_frag_insert_packet(orig_node_src, *skb, &head))
		goto out_err;

	/* Leave if more fragments are needed to merge. */
	if (hlist_empty(&head))
		goto out;

	skb_out = batadv_frag_merge_packets(&head, *skb);
	if (!skb_out)
		goto out_err;

out:
	*skb = skb_out;
	ret = true;
out_err:
	return ret;
}
