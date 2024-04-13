static int x25_getname(struct socket *sock, struct sockaddr *uaddr,
		       int *uaddr_len, int peer)
{
	struct sockaddr_x25 *sx25 = (struct sockaddr_x25 *)uaddr;
	struct sock *sk = sock->sk;
	struct x25_sock *x25 = x25_sk(sk);
	int rc = 0;

	if (peer) {
		if (sk->sk_state != TCP_ESTABLISHED) {
			rc = -ENOTCONN;
			goto out;
		}
		sx25->sx25_addr = x25->dest_addr;
	} else
		sx25->sx25_addr = x25->source_addr;

	sx25->sx25_family = AF_X25;
	*uaddr_len = sizeof(*sx25);

out:
	return rc;
}
