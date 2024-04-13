__ip_vs_svc_fwm_find(struct net *net, int af, __u32 fwmark)
{
	unsigned int hash;
	struct ip_vs_service *svc;

	/* Check for fwmark addressed entries */
	hash = ip_vs_svc_fwm_hashkey(net, fwmark);

	list_for_each_entry(svc, &ip_vs_svc_fwm_table[hash], f_list) {
		if (svc->fwmark == fwmark && svc->af == af
		    && net_eq(svc->net, net)) {
			/* HIT */
			return svc;
		}
	}

	return NULL;
}
