static void keep_sockalive(SOCKETTYPE fd)
{
	const int tcp_one = 1;
#ifndef WIN32
	const int tcp_keepidle = 45;
	const int tcp_keepintvl = 30;
	int flags = fcntl(fd, F_GETFL, 0);

	fcntl(fd, F_SETFL, O_NONBLOCK | flags);
#else
	u_long flags = 1;

	ioctlsocket(fd, FIONBIO, &flags);
#endif

	setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (const char *)&tcp_one, sizeof(tcp_one));
	if (!opt_delaynet)
#ifndef __linux
		setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char *)&tcp_one, sizeof(tcp_one));
#else /* __linux */
		setsockopt(fd, SOL_TCP, TCP_NODELAY, (const void *)&tcp_one, sizeof(tcp_one));
	setsockopt(fd, SOL_TCP, TCP_KEEPCNT, &tcp_one, sizeof(tcp_one));
	setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, &tcp_keepidle, sizeof(tcp_keepidle));
	setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, &tcp_keepintvl, sizeof(tcp_keepintvl));
#endif /* __linux__ */

#ifdef __APPLE_CC__
	setsockopt(fd, IPPROTO_TCP, TCP_KEEPALIVE, &tcp_keepintvl, sizeof(tcp_keepintvl));
#endif /* __APPLE_CC__ */

}
