static void sctp_v4_inaddr_any(union sctp_addr *addr, __be16 port)
{
	addr->v4.sin_family = AF_INET;
	addr->v4.sin_addr.s_addr = htonl(INADDR_ANY);
	addr->v4.sin_port = port;
}
