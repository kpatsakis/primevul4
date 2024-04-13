char *get_proxy(char *url, struct pool *pool)
{
	pool->rpc_proxy = NULL;

	char *split;
	int plen, len, i;

	for (i = 0; proxynames[i].name; i++) {
		plen = strlen(proxynames[i].name);
		if (strncmp(url, proxynames[i].name, plen) == 0) {
			if (!(split = strchr(url, '|')))
				return url;

			*split = '\0';
			len = split - url;
			pool->rpc_proxy = malloc(1 + len - plen);
			if (!(pool->rpc_proxy))
				quithere(1, "Failed to malloc rpc_proxy");

			strcpy(pool->rpc_proxy, url + plen);
			extract_sockaddr(pool->rpc_proxy, &pool->sockaddr_proxy_url, &pool->sockaddr_proxy_port);
			pool->rpc_proxytype = proxynames[i].proxytype;
			url = split + 1;
			break;
		}
	}
	return url;
}
