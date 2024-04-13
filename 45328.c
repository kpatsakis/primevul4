static int l2tp_ip_disconnect(struct sock *sk, int flags)
{
	if (sock_flag(sk, SOCK_ZAPPED))
		return 0;

	return udp_disconnect(sk, flags);
}
