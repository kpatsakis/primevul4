static unsigned int x25_new_lci(struct x25_neigh *nb)
{
	unsigned int lci = 1;
	struct sock *sk;

	read_lock_bh(&x25_list_lock);

	while ((sk = __x25_find_socket(lci, nb)) != NULL) {
		sock_put(sk);
		if (++lci == 4096) {
			lci = 0;
			break;
		}
	}

	read_unlock_bh(&x25_list_lock);
	return lci;
}
