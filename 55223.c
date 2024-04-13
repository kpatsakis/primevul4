static void sctp_destruct_sock(struct sock *sk)
{
	struct sctp_sock *sp = sctp_sk(sk);

	/* Free up the HMAC transform. */
	crypto_free_shash(sp->hmac);

	inet_sock_destruct(sk);
}
