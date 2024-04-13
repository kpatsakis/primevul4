static void block_socket(SOCKETTYPE fd)
{
#ifndef WIN32
	int flags = fcntl(fd, F_GETFL, 0);

	fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
#else
	u_long flags = 0;

	ioctlsocket(fd, FIONBIO, &flags);
#endif
}
