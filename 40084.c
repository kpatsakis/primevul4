static int ax25_listen(struct socket *sock, int backlog)
{
	struct sock *sk = sock->sk;
	int res = 0;

	lock_sock(sk);
	if (sk->sk_type == SOCK_SEQPACKET && sk->sk_state != TCP_LISTEN) {
		sk->sk_max_ack_backlog = backlog;
		sk->sk_state           = TCP_LISTEN;
		goto out;
	}
	res = -EOPNOTSUPP;

out:
	release_sock(sk);

	return res;
}
