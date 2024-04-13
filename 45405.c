static int atalk_pick_and_bind_port(struct sock *sk, struct sockaddr_at *sat)
{
	int retval;

	write_lock_bh(&atalk_sockets_lock);

	for (sat->sat_port = ATPORT_RESERVED;
	     sat->sat_port < ATPORT_LAST;
	     sat->sat_port++) {
		struct sock *s;

		sk_for_each(s, &atalk_sockets) {
			struct atalk_sock *at = at_sk(s);

			if (at->src_net == sat->sat_addr.s_net &&
			    at->src_node == sat->sat_addr.s_node &&
			    at->src_port == sat->sat_port)
				goto try_next_port;
		}

		/* Wheee, it's free, assign and insert. */
		__atalk_insert_socket(sk);
		at_sk(sk)->src_port = sat->sat_port;
		retval = 0;
		goto out;

try_next_port:;
	}

	retval = -EBUSY;
out:
	write_unlock_bh(&atalk_sockets_lock);
	return retval;
}
