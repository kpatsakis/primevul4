static int rawsock_add_header(struct sk_buff *skb)
{
	*skb_push(skb, NFC_HEADER_SIZE) = 0;

	return 0;
}
