ip_vs_trash_get_dest(struct ip_vs_service *svc, const union nf_inet_addr *daddr,
		     __be16 dport)
{
	struct ip_vs_dest *dest, *nxt;
	struct netns_ipvs *ipvs = net_ipvs(svc->net);

	/*
	 * Find the destination in trash
	 */
	list_for_each_entry_safe(dest, nxt, &ipvs->dest_trash, n_list) {
		IP_VS_DBG_BUF(3, "Destination %u/%s:%u still in trash, "
			      "dest->refcnt=%d\n",
			      dest->vfwmark,
			      IP_VS_DBG_ADDR(svc->af, &dest->addr),
			      ntohs(dest->port),
			      atomic_read(&dest->refcnt));
		if (dest->af == svc->af &&
		    ip_vs_addr_equal(svc->af, &dest->addr, daddr) &&
		    dest->port == dport &&
		    dest->vfwmark == svc->fwmark &&
		    dest->protocol == svc->protocol &&
		    (svc->fwmark ||
		     (ip_vs_addr_equal(svc->af, &dest->vaddr, &svc->addr) &&
		      dest->vport == svc->port))) {
			/* HIT */
			return dest;
		}

		/*
		 * Try to purge the destination from trash if not referenced
		 */
		if (atomic_read(&dest->refcnt) == 1) {
			IP_VS_DBG_BUF(3, "Removing destination %u/%s:%u "
				      "from trash\n",
				      dest->vfwmark,
				      IP_VS_DBG_ADDR(svc->af, &dest->addr),
				      ntohs(dest->port));
			list_del(&dest->n_list);
			ip_vs_dst_reset(dest);
			__ip_vs_unbind_svc(dest);
			free_percpu(dest->stats.cpustats);
			kfree(dest);
		}
	}

	return NULL;
}
