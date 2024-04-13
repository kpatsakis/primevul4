static int sco_sock_listen(struct socket *sock, int backlog)
{
	struct sock *sk = sock->sk;
	bdaddr_t *src = &sco_pi(sk)->src;
	int err = 0;

	BT_DBG("sk %p backlog %d", sk, backlog);

	lock_sock(sk);

	if (sk->sk_state != BT_BOUND) {
		err = -EBADFD;
		goto done;
	}

	if (sk->sk_type != SOCK_SEQPACKET) {
		err = -EINVAL;
		goto done;
	}

	write_lock(&sco_sk_list.lock);

	if (__sco_get_sock_listen_by_addr(src)) {
		err = -EADDRINUSE;
		goto unlock;
	}

	sk->sk_max_ack_backlog = backlog;
	sk->sk_ack_backlog = 0;

	sk->sk_state = BT_LISTEN;

unlock:
	write_unlock(&sco_sk_list.lock);

done:
	release_sock(sk);
	return err;
}
