bool sk_net_capable(const struct sock *sk, int cap)
{
	return sk_ns_capable(sk, sock_net(sk)->user_ns, cap);
}
