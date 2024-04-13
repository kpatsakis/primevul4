static inline bool tcp_can_repair_sock(const struct sock *sk)
{
	return ns_capable(sock_net(sk)->user_ns, CAP_NET_ADMIN) &&
		(sk->sk_state != TCP_LISTEN);
}
