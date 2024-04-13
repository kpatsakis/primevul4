static int ip_vs_svc_hash(struct ip_vs_service *svc)
{
	unsigned int hash;

	if (svc->flags & IP_VS_SVC_F_HASHED) {
		pr_err("%s(): request for already hashed, called from %pF\n",
		       __func__, __builtin_return_address(0));
		return 0;
	}

	if (svc->fwmark == 0) {
		/*
		 *  Hash it by <netns,protocol,addr,port> in ip_vs_svc_table
		 */
		hash = ip_vs_svc_hashkey(svc->net, svc->af, svc->protocol,
					 &svc->addr, svc->port);
		list_add(&svc->s_list, &ip_vs_svc_table[hash]);
	} else {
		/*
		 *  Hash it by fwmark in svc_fwm_table
		 */
		hash = ip_vs_svc_fwm_hashkey(svc->net, svc->fwmark);
		list_add(&svc->f_list, &ip_vs_svc_fwm_table[hash]);
	}

	svc->flags |= IP_VS_SVC_F_HASHED;
	/* increase its refcnt because it is referenced by the svc table */
	atomic_inc(&svc->refcnt);
	return 1;
}
