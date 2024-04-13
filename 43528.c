static bool sock_connecting(void)
{
#ifndef WIN32
	return errno == EINPROGRESS;
#else
	return WSAGetLastError() == WSAEWOULDBLOCK;
#endif
}
