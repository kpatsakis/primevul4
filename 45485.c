static struct sock *__ipxitf_find_socket(struct ipx_interface *intrfc,
					 __be16 port)
{
	struct sock *s;

	sk_for_each(s, &intrfc->if_sklist)
		if (ipx_sk(s)->port == port)
			goto found;
	s = NULL;
found:
	return s;
}
