ip_vs_add_dest(struct ip_vs_service *svc, struct ip_vs_dest_user_kern *udest)
{
	struct ip_vs_dest *dest;
	union nf_inet_addr daddr;
	__be16 dport = udest->port;
	int ret;

	EnterFunction(2);

	if (udest->weight < 0) {
		pr_err("%s(): server weight less than zero\n", __func__);
		return -ERANGE;
	}

	if (udest->l_threshold > udest->u_threshold) {
		pr_err("%s(): lower threshold is higher than upper threshold\n",
			__func__);
		return -ERANGE;
	}

	ip_vs_addr_copy(svc->af, &daddr, &udest->addr);

	/*
	 * Check if the dest already exists in the list
	 */
	dest = ip_vs_lookup_dest(svc, &daddr, dport);

	if (dest != NULL) {
		IP_VS_DBG(1, "%s(): dest already exists\n", __func__);
		return -EEXIST;
	}

	/*
	 * Check if the dest already exists in the trash and
	 * is from the same service
	 */
	dest = ip_vs_trash_get_dest(svc, &daddr, dport);

	if (dest != NULL) {
		IP_VS_DBG_BUF(3, "Get destination %s:%u from trash, "
			      "dest->refcnt=%d, service %u/%s:%u\n",
			      IP_VS_DBG_ADDR(svc->af, &daddr), ntohs(dport),
			      atomic_read(&dest->refcnt),
			      dest->vfwmark,
			      IP_VS_DBG_ADDR(svc->af, &dest->vaddr),
			      ntohs(dest->vport));

		/*
		 * Get the destination from the trash
		 */
		list_del(&dest->n_list);

		__ip_vs_update_dest(svc, dest, udest, 1);
		ret = 0;
	} else {
		/*
		 * Allocate and initialize the dest structure
		 */
		ret = ip_vs_new_dest(svc, udest, &dest);
	}
	LeaveFunction(2);

	return ret;
}
