int socket_connect(const char *addr, uint16_t port)
{
	int sfd = -1;
	int yes = 1;
	struct hostent *hp;
	struct sockaddr_in saddr;
#ifdef WIN32
	WSADATA wsa_data;
	if (!wsa_init) {
		if (WSAStartup(MAKEWORD(2,2), &wsa_data) != ERROR_SUCCESS) {
			fprintf(stderr, "WSAStartup failed!\n");
			ExitProcess(-1);
		}
		wsa_init = 1;
	}
#endif

	if (!addr) {
		errno = EINVAL;
		return -1;
	}

	if ((hp = gethostbyname(addr)) == NULL) {
		if (verbose >= 2)
			fprintf(stderr, "%s: unknown host '%s'\n", __func__, addr);
		return -1;
	}

	if (!hp->h_addr) {
		if (verbose >= 2)
			fprintf(stderr, "%s: gethostbyname returned NULL address!\n",
					__func__);
		return -1;
	}

	if (0 > (sfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))) {
		perror("socket()");
		return -1;
	}

	if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (void*)&yes, sizeof(int)) == -1) {
		perror("setsockopt()");
		socket_close(sfd);
		return -1;
	}

	memset((void *) &saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = *(uint32_t *) hp->h_addr;
	saddr.sin_port = htons(port);

	if (connect(sfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
		perror("connect");
		socket_close(sfd);
		return -2;
	}

	return sfd;
}
