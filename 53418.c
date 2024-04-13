static struct sock *__rfcomm_get_listen_sock_by_addr(u8 channel, bdaddr_t *src)
{
	struct sock *sk = NULL;

	sk_for_each(sk, &rfcomm_sk_list.head) {
		if (rfcomm_pi(sk)->channel != channel)
			continue;

		if (bacmp(&rfcomm_pi(sk)->src, src))
			continue;

		if (sk->sk_state == BT_BOUND || sk->sk_state == BT_LISTEN)
			break;
	}

	return sk ? sk : NULL;
}
