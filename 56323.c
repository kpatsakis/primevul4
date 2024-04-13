win32_gethostbyaddr(const char *addr, int len, int type)
{
	static struct hostent host;
	static char hostbuf[NI_MAXHOST];
	char hname[NI_MAXHOST];
	struct sockaddr_in6 addr6;

	host.h_name = hostbuf;
	switch (type) {
	case AF_INET:
		return gethostbyaddr(addr, len, type);
		break;
	case AF_INET6:
		memset(&addr6, 0, sizeof(addr6));
		addr6.sin6_family = AF_INET6;
		memcpy(&addr6.sin6_addr, addr, len);
		if (getnameinfo((struct sockaddr *)&addr6, sizeof(addr6),
		    hname, sizeof(hname), NULL, 0, 0)) {
			return NULL;
		} else {
			strcpy(host.h_name, hname);
			return &host;
		}
		break;
	default:
		return NULL;
	}
}
