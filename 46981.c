static int sctp_inet_supported_addrs(const struct sctp_sock *opt,
				     __be16 *types)
{
	types[0] = SCTP_PARAM_IPV4_ADDRESS;
	return 1;
}
