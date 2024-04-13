static void skb_free_head(struct sk_buff *skb)
{
	unsigned char *head = skb->head;

	if (skb->head_frag)
		skb_free_frag(head);
	else
		kfree(head);
}
