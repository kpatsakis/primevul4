static struct sock *atalk_find_or_insert_socket(struct sock *sk,
						struct sockaddr_at *sat)
{
	struct sock *s;
	struct atalk_sock *at;

	write_lock_bh(&atalk_sockets_lock);
	sk_for_each(s, &atalk_sockets) {
		at = at_sk(s);

		if (at->src_net == sat->sat_addr.s_net &&
		    at->src_node == sat->sat_addr.s_node &&
		    at->src_port == sat->sat_port)
			goto found;
	}
	s = NULL;
	__atalk_insert_socket(sk); /* Wheee, it's free, assign and insert. */
found:
	write_unlock_bh(&atalk_sockets_lock);
	return s;
}
