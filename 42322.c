static struct sock *__rfcomm_get_sock_by_addr(u8 channel, bdaddr_t *src)
{
	struct sock *sk = NULL;
	struct hlist_node *node;

	sk_for_each(sk, node, &rfcomm_sk_list.head) {
		if (rfcomm_pi(sk)->channel == channel &&
				!bacmp(&bt_sk(sk)->src, src))
			break;
	}

	return node ? sk : NULL;
}
