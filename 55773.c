int tcp_sendpage(struct sock *sk, struct page *page, int offset,
		 size_t size, int flags)
{
	ssize_t res;

	if (!(sk->sk_route_caps & NETIF_F_SG) ||
	    !sk_check_csum_caps(sk))
		return sock_no_sendpage(sk->sk_socket, page, offset, size,
					flags);

	lock_sock(sk);

	tcp_rate_check_app_limited(sk);  /* is sending application-limited? */

	res = do_tcp_sendpages(sk, page, offset, size, flags);
	release_sock(sk);
	return res;
}
