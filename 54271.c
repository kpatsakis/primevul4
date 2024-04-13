traverse_proxy (int proxy_type, int print_fd, int sok, char *ip, int port, struct msproxy_state_t *state, netstore *ns_proxy, int csok4, int csok6, int *csok, char bound)
{
	switch (proxy_type)
	{
	case 1:
		return traverse_wingate (print_fd, sok, ip, port);
	case 2:
		return traverse_socks (print_fd, sok, ip, port);
	case 3:
		return traverse_socks5 (print_fd, sok, ip, port);
	case 4:
		return traverse_http (print_fd, sok, ip, port);
#ifdef USE_MSPROXY
	case 5:
		return traverse_msproxy (sok, ip, port, state, ns_proxy, csok4, csok6, csok, bound);
#endif
	}

	return 1;
}
