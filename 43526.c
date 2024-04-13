static bool setup_stratum_socket(struct pool *pool)
{
	struct addrinfo servinfobase, *servinfo, *hints, *p;
	char *sockaddr_url, *sockaddr_port;
	int sockd;
	int ret;

	mutex_lock(&pool->stratum_lock);
	pool->stratum_active = false;
	if (pool->sock) {
		/* FIXME: change to LOG_DEBUG if issue #88 resolved */
		applog(LOG_INFO, "Closing %s socket", get_pool_name(pool));
		CLOSESOCKET(pool->sock);
	}
	pool->sock = 0;
	mutex_unlock(&pool->stratum_lock);

	hints = &pool->stratum_hints;
	memset(hints, 0, sizeof(struct addrinfo));
	hints->ai_family = AF_UNSPEC;
	hints->ai_socktype = SOCK_STREAM;
	servinfo = &servinfobase;

	if (!pool->rpc_proxy && opt_socks_proxy) {
		pool->rpc_proxy = opt_socks_proxy;
		extract_sockaddr(pool->rpc_proxy, &pool->sockaddr_proxy_url, &pool->sockaddr_proxy_port);
		pool->rpc_proxytype = PROXY_SOCKS5;
	}

	if (pool->rpc_proxy) {
		sockaddr_url = pool->sockaddr_proxy_url;
		sockaddr_port = pool->sockaddr_proxy_port;
	} else {
		sockaddr_url = pool->sockaddr_url;
		sockaddr_port = pool->stratum_port;
	}

	ret = getaddrinfo(sockaddr_url, sockaddr_port, hints, &servinfo);
	if (ret) {
		applog(LOG_INFO, "getaddrinfo() in setup_stratum_socket() returned %i: %s", ret, gai_strerror(ret));
		if (!pool->probed) {
			applog(LOG_WARNING, "Failed to resolve (wrong URL?) %s:%s",
			       sockaddr_url, sockaddr_port);
			pool->probed = true;
		} else {
			applog(LOG_INFO, "Failed to getaddrinfo for %s:%s",
			       sockaddr_url, sockaddr_port);
		}
		return false;
	}

	for (p = servinfo; p != NULL; p = p->ai_next) {
		sockd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sockd == -1) {
			applog(LOG_DEBUG, "Failed socket");
			continue;
		}

		/* Iterate non blocking over entries returned by getaddrinfo
		 * to cope with round robin DNS entries, finding the first one
		 * we can connect to quickly. */
		noblock_socket(sockd);
		if (connect(sockd, p->ai_addr, p->ai_addrlen) == -1) {
			struct timeval tv_timeout = {1, 0};
			int selret;
			fd_set rw;

			if (!sock_connecting()) {
				CLOSESOCKET(sockd);
				applog(LOG_DEBUG, "Failed sock connect");
				continue;
			}
retry:
			FD_ZERO(&rw);
			FD_SET(sockd, &rw);
			selret = select(sockd + 1, NULL, &rw, NULL, &tv_timeout);
			if  (selret > 0 && FD_ISSET(sockd, &rw)) {
				socklen_t len;
				int err, n;

				len = sizeof(err);
				n = getsockopt(sockd, SOL_SOCKET, SO_ERROR, (char *)&err, &len);
				if (!n && !err) {
					applog(LOG_DEBUG, "Succeeded delayed connect");
					block_socket(sockd);
					break;
				}
			}
			if (selret < 0 && interrupted())
				goto retry;
			CLOSESOCKET(sockd);
			applog(LOG_DEBUG, "Select timeout/failed connect");
			continue;
		}
		applog(LOG_WARNING, "Succeeded immediate connect");
		block_socket(sockd);

		break;
	}
	if (p == NULL) {
		applog(LOG_INFO, "Failed to connect to stratum on %s:%s",
		       sockaddr_url, sockaddr_port);
		freeaddrinfo(servinfo);
		return false;
	}
	freeaddrinfo(servinfo);

	if (pool->rpc_proxy) {
		switch (pool->rpc_proxytype) {
			case PROXY_HTTP_1_0:
				if (!http_negotiate(pool, sockd, true))
					return false;
				break;
			case PROXY_HTTP:
				if (!http_negotiate(pool, sockd, false))
					return false;
				break;
			case PROXY_SOCKS5:
			case PROXY_SOCKS5H:
				if (!socks5_negotiate(pool, sockd))
					return false;
				break;
			case PROXY_SOCKS4:
				if (!socks4_negotiate(pool, sockd, false))
					return false;
				break;
			case PROXY_SOCKS4A:
				if (!socks4_negotiate(pool, sockd, true))
					return false;
				break;
			default:
				applog(LOG_WARNING, "Unsupported proxy type for %s:%s",
				       pool->sockaddr_proxy_url, pool->sockaddr_proxy_port);
				return false;
				break;
		}
	}

	if (!pool->sockbuf) {
		pool->sockbuf = (char *)calloc(RBUFSIZE, 1);
		if (!pool->sockbuf)
			quithere(1, "Failed to calloc pool sockbuf");
		pool->sockbuf_size = RBUFSIZE;
	}

	pool->sock = sockd;
	keep_sockalive(sockd);
	return true;
}
