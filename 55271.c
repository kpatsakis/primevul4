static void sctp_wfree(struct sk_buff *skb)
{
	struct sctp_chunk *chunk = skb_shinfo(skb)->destructor_arg;
	struct sctp_association *asoc = chunk->asoc;
	struct sock *sk = asoc->base.sk;

	asoc->sndbuf_used -= SCTP_DATA_SNDSIZE(chunk) +
				sizeof(struct sk_buff) +
				sizeof(struct sctp_chunk);

	WARN_ON(refcount_sub_and_test(sizeof(struct sctp_chunk), &sk->sk_wmem_alloc));

	/*
	 * This undoes what is done via sctp_set_owner_w and sk_mem_charge
	 */
	sk->sk_wmem_queued   -= skb->truesize;
	sk_mem_uncharge(sk, skb->truesize);

	sock_wfree(skb);
	sctp_wake_up_waiters(sk, asoc);

	sctp_association_put(asoc);
}
