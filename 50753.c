static struct dst_entry *tcp_v4_route_req(const struct sock *sk,
					  struct flowi *fl,
					  const struct request_sock *req,
					  bool *strict)
{
	struct dst_entry *dst = inet_csk_route_req(sk, &fl->u.ip4, req);

	if (strict) {
		if (fl->u.ip4.daddr == inet_rsk(req)->ir_rmt_addr)
			*strict = true;
		else
			*strict = false;
	}

	return dst;
}
