static struct sock *__unix_find_socket_byname(struct net *net,
					      struct sockaddr_un *sunname,
					      int len, int type, unsigned int hash)
{
	struct sock *s;

	sk_for_each(s, &unix_socket_table[hash ^ type]) {
		struct unix_sock *u = unix_sk(s);

		if (!net_eq(sock_net(s), net))
			continue;

		if (u->addr->len == len &&
		    !memcmp(u->addr->name, sunname, len))
			goto found;
	}
	s = NULL;
found:
	return s;
}
