bool rxrpc_kernel_is_data_last(struct sk_buff *skb)
{
	struct rxrpc_skb_priv *sp = rxrpc_skb(skb);

	ASSERTCMP(skb->mark, ==, RXRPC_SKB_MARK_DATA);

	return sp->hdr.flags & RXRPC_LAST_PACKET;
}
