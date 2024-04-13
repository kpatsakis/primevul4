void tipc_sk_mcast_rcv(struct net *net, struct sk_buff_head *arrvq,
		       struct sk_buff_head *inputq)
{
	struct tipc_msg *msg;
	struct tipc_plist dports;
	u32 portid;
	u32 scope = TIPC_CLUSTER_SCOPE;
	struct sk_buff_head tmpq;
	uint hsz;
	struct sk_buff *skb, *_skb;

	__skb_queue_head_init(&tmpq);
	tipc_plist_init(&dports);

	skb = tipc_skb_peek(arrvq, &inputq->lock);
	for (; skb; skb = tipc_skb_peek(arrvq, &inputq->lock)) {
		msg = buf_msg(skb);
		hsz = skb_headroom(skb) + msg_hdr_sz(msg);

		if (in_own_node(net, msg_orignode(msg)))
			scope = TIPC_NODE_SCOPE;

		/* Create destination port list and message clones: */
		tipc_nametbl_mc_translate(net,
					  msg_nametype(msg), msg_namelower(msg),
					  msg_nameupper(msg), scope, &dports);
		portid = tipc_plist_pop(&dports);
		for (; portid; portid = tipc_plist_pop(&dports)) {
			_skb = __pskb_copy(skb, hsz, GFP_ATOMIC);
			if (_skb) {
				msg_set_destport(buf_msg(_skb), portid);
				__skb_queue_tail(&tmpq, _skb);
				continue;
			}
			pr_warn("Failed to clone mcast rcv buffer\n");
		}
		/* Append to inputq if not already done by other thread */
		spin_lock_bh(&inputq->lock);
		if (skb_peek(arrvq) == skb) {
			skb_queue_splice_tail_init(&tmpq, inputq);
			kfree_skb(__skb_dequeue(arrvq));
		}
		spin_unlock_bh(&inputq->lock);
		__skb_queue_purge(&tmpq);
		kfree_skb(skb);
	}
	tipc_sk_rcv(net, inputq);
}
