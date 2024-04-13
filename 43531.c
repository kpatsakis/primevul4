static bool socks4_negotiate(struct pool *pool, int sockd, bool socks4a)
{
	unsigned short port;
	in_addr_t inp;
	char buf[515];
	int i, len;
	int ret;

	buf[0] = 0x04;
	buf[1] = 0x01;
	port = atoi(pool->stratum_port);
	buf[2] = port >> 8;
	buf[3] = port & 0xff;
	sprintf(&buf[8], "SGMINER");

	/* See if we've been given an IP address directly to avoid needing to
	 * resolve it. */
	inp = inet_addr(pool->sockaddr_url);
	inp = ntohl(inp);
	if ((int)inp != -1)
		socks4a = false;
	else {
		/* Try to extract the IP address ourselves first */
		struct addrinfo servinfobase, *servinfo, hints;

		servinfo = &servinfobase;
		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_family = AF_INET; /* IPV4 only */
		ret = getaddrinfo(pool->sockaddr_url, NULL, &hints, &servinfo);
		if (!ret) {
			applog(LOG_ERR, "getaddrinfo() in socks4_negotiate() returned %i: %s", ret, gai_strerror(ret));

			struct sockaddr_in *saddr_in = (struct sockaddr_in *)servinfo->ai_addr;

			inp = ntohl(saddr_in->sin_addr.s_addr);
			socks4a = false;
			freeaddrinfo(servinfo);
		}
	}

	if (!socks4a) {
		if ((int)inp == -1) {
			applog(LOG_WARNING, "Invalid IP address specified for socks4 proxy: %s",
			       pool->sockaddr_url);
			return false;
		}
		buf[4] = (inp >> 24) & 0xFF;
		buf[5] = (inp >> 16) & 0xFF;
		buf[6] = (inp >>  8) & 0xFF;
		buf[7] = (inp >>  0) & 0xFF;
		send(sockd, buf, 16, 0);
	} else {
		/* This appears to not be working but hopefully most will be
		 * able to resolve IP addresses themselves. */
		buf[4] = 0;
		buf[5] = 0;
		buf[6] = 0;
		buf[7] = 1;
		len = strlen(pool->sockaddr_url);
		if (len > 255)
			len = 255;
		memcpy(&buf[16], pool->sockaddr_url, len);
		len += 16;
		buf[len++] = '\0';
		send(sockd, buf, len, 0);
	}

	if (recv_byte(sockd) != 0x00 || recv_byte(sockd) != 0x5a) {
		applog(LOG_WARNING, "Bad response from %s:%s SOCKS4 server",
		       pool->sockaddr_proxy_url, pool->sockaddr_proxy_port);
		return false;
	}

	for (i = 0; i < 6; i++)
		recv_byte(sockd);

	return true;
}
