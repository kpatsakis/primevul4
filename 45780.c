static struct sock *__x25_find_socket(unsigned int lci, struct x25_neigh *nb)
{
	struct sock *s;

	sk_for_each(s, &x25_list)
		if (x25_sk(s)->lci == lci && x25_sk(s)->neighbour == nb) {
			sock_hold(s);
			goto found;
		}
	s = NULL;
found:
	return s;
}
