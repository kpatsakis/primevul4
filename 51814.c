int socket_close(int fd) {
#ifdef WIN32
	return closesocket(fd);
#else
	return close(fd);
#endif
}
