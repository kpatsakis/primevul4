static struct sock *__rfcomm_get_sock_by_addr(u8 channel, bdaddr_t *src)
{
	struct sock *sk = NULL;

	sk_for_each(sk, &rfcomm_sk_list.head) {
		if (rfcomm_pi(sk)->channel == channel &&
				!bacmp(&rfcomm_pi(sk)->src, src))
			break;
	}

	return sk ? sk : NULL;
}
