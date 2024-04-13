void rxrpc_kernel_data_delivered(struct sk_buff *skb)
{
	struct rxrpc_skb_priv *sp = rxrpc_skb(skb);
	struct rxrpc_call *call = sp->call;

	ASSERTCMP(ntohl(sp->hdr.seq), >=, call->rx_data_recv);
	ASSERTCMP(ntohl(sp->hdr.seq), <=, call->rx_data_recv + 1);
	call->rx_data_recv = ntohl(sp->hdr.seq);

	ASSERTCMP(ntohl(sp->hdr.seq), >, call->rx_data_eaten);
	rxrpc_free_skb(skb);
}
