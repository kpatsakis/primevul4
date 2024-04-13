static struct sock *rfcomm_get_sock_by_channel(int state, u8 channel, bdaddr_t *src)
{
	struct sock *sk = NULL, *sk1 = NULL;

	read_lock(&rfcomm_sk_list.lock);

	sk_for_each(sk, &rfcomm_sk_list.head) {
		if (state && sk->sk_state != state)
			continue;

		if (rfcomm_pi(sk)->channel == channel) {
			/* Exact match. */
			if (!bacmp(&rfcomm_pi(sk)->src, src))
				break;

			/* Closest match */
			if (!bacmp(&rfcomm_pi(sk)->src, BDADDR_ANY))
				sk1 = sk;
		}
	}

	read_unlock(&rfcomm_sk_list.lock);

	return sk ? sk : sk1;
}
