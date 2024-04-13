static int sctp_inet_bind_verify(struct sctp_sock *opt, union sctp_addr *addr)
{
	return sctp_v4_available(addr, opt);
}
