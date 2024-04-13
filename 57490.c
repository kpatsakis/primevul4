void ip6_flush_pending_frames(struct sock *sk)
{
	__ip6_flush_pending_frames(sk, &sk->sk_write_queue,
				   &inet_sk(sk)->cork, &inet6_sk(sk)->cork);
}
