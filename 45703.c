u32 rxrpc_kernel_get_abort_code(struct sk_buff *skb)
{
	struct rxrpc_skb_priv *sp = rxrpc_skb(skb);

	ASSERTCMP(skb->mark, ==, RXRPC_SKB_MARK_REMOTE_ABORT);

	return sp->call->abort_code;
}
