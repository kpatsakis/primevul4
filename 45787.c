static int x25_bind(struct socket *sock, struct sockaddr *uaddr, int addr_len)
{
	struct sock *sk = sock->sk;
	struct sockaddr_x25 *addr = (struct sockaddr_x25 *)uaddr;
	int len, i, rc = 0;

	if (!sock_flag(sk, SOCK_ZAPPED) ||
	    addr_len != sizeof(struct sockaddr_x25) ||
	    addr->sx25_family != AF_X25) {
		rc = -EINVAL;
		goto out;
	}

	len = strlen(addr->sx25_addr.x25_addr);
	for (i = 0; i < len; i++) {
		if (!isdigit(addr->sx25_addr.x25_addr[i])) {
			rc = -EINVAL;
			goto out;
		}
	}

	lock_sock(sk);
	x25_sk(sk)->source_addr = addr->sx25_addr;
	x25_insert_socket(sk);
	sock_reset_flag(sk, SOCK_ZAPPED);
	release_sock(sk);
	SOCK_DEBUG(sk, "x25_bind: socket is bound\n");
out:
	return rc;
}
