struct sock *x25_find_socket(unsigned int lci, struct x25_neigh *nb)
{
	struct sock *s;

	read_lock_bh(&x25_list_lock);
	s = __x25_find_socket(lci, nb);
	read_unlock_bh(&x25_list_lock);
	return s;
}
