match_address(struct TCP_Server_Info *server, struct sockaddr *addr)
{
	struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
	struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;

	switch (addr->sa_family) {
	case AF_INET:
		if (addr4->sin_addr.s_addr !=
		    server->addr.sockAddr.sin_addr.s_addr)
			return false;
		if (addr4->sin_port &&
		    addr4->sin_port != server->addr.sockAddr.sin_port)
			return false;
		break;
	case AF_INET6:
		if (!ipv6_addr_equal(&addr6->sin6_addr,
				     &server->addr.sockAddr6.sin6_addr))
			return false;
		if (addr6->sin6_scope_id !=
		    server->addr.sockAddr6.sin6_scope_id)
			return false;
		if (addr6->sin6_port &&
		    addr6->sin6_port != server->addr.sockAddr6.sin6_port)
			return false;
		break;
	}

	return true;
}
