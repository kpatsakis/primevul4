static void sctp_wfree(struct sk_buff *skb)
{
	struct sctp_association *asoc;
	struct sctp_chunk *chunk;
	struct sock *sk;

	/* Get the saved chunk pointer.  */
	chunk = *((struct sctp_chunk **)(skb->cb));
	asoc = chunk->asoc;
	sk = asoc->base.sk;
	asoc->sndbuf_used -= SCTP_DATA_SNDSIZE(chunk) +
				sizeof(struct sk_buff) +
				sizeof(struct sctp_chunk);

	atomic_sub(sizeof(struct sctp_chunk), &sk->sk_wmem_alloc);

	sock_wfree(skb);
	__sctp_write_space(asoc);

	sctp_association_put(asoc);
}
