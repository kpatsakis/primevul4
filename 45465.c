static struct sock *__sco_get_sock_listen_by_addr(bdaddr_t *ba)
{
	struct sock *sk;

	sk_for_each(sk, &sco_sk_list.head) {
		if (sk->sk_state != BT_LISTEN)
			continue;

		if (!bacmp(&sco_pi(sk)->src, ba))
			return sk;
	}

	return NULL;
}
