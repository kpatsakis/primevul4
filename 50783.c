static struct dst_entry *tcp_v6_route_req(const struct sock *sk,
					  struct flowi *fl,
					  const struct request_sock *req,
					  bool *strict)
{
	if (strict)
		*strict = true;
	return inet6_csk_route_req(sk, &fl->u.ip6, req, IPPROTO_TCP);
}
