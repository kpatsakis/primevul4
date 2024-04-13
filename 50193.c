static bool sock_needs_netstamp(const struct sock *sk)
{
	switch (sk->sk_family) {
	case AF_UNSPEC:
	case AF_UNIX:
		return false;
	default:
		return true;
	}
}
