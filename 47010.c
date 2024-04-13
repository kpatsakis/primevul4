static int sctp_v4_to_addr_param(const union sctp_addr *addr,
				 union sctp_addr_param *param)
{
	int length = sizeof(sctp_ipv4addr_param_t);

	param->v4.param_hdr.type = SCTP_PARAM_IPV4_ADDRESS;
	param->v4.param_hdr.length = htons(length);
	param->v4.addr.s_addr = addr->v4.sin_addr.s_addr;

	return length;
}
