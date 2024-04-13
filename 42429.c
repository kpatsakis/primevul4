static int ip_vs_rs_hash(struct netns_ipvs *ipvs, struct ip_vs_dest *dest)
{
	unsigned int hash;

	if (!list_empty(&dest->d_list)) {
		return 0;
	}

	/*
	 *	Hash by proto,addr,port,
	 *	which are the parameters of the real service.
	 */
	hash = ip_vs_rs_hashkey(dest->af, &dest->addr, dest->port);

	list_add(&dest->d_list, &ipvs->rs_table[hash]);

	return 1;
}
