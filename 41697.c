static void sctp_skb_set_owner_r_frag(struct sk_buff *skb, struct sock *sk)
{
	struct sk_buff *frag;

	if (!skb->data_len)
		goto done;

	/* Don't forget the fragments. */
	skb_walk_frags(skb, frag)
		sctp_skb_set_owner_r_frag(frag, sk);

done:
	sctp_skb_set_owner_r(skb, sk);
}
