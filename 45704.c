int rxrpc_kernel_get_error_number(struct sk_buff *skb)
{
	struct rxrpc_skb_priv *sp = rxrpc_skb(skb);

	return sp->error;
}
