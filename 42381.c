static void __ip_vs_del_service(struct ip_vs_service *svc)
{
	struct ip_vs_dest *dest, *nxt;
	struct ip_vs_scheduler *old_sched;
	struct ip_vs_pe *old_pe;
	struct netns_ipvs *ipvs = net_ipvs(svc->net);

	pr_info("%s: enter\n", __func__);

	/* Count only IPv4 services for old get/setsockopt interface */
	if (svc->af == AF_INET)
		ipvs->num_services--;

	ip_vs_stop_estimator(svc->net, &svc->stats);

	/* Unbind scheduler */
	old_sched = svc->scheduler;
	ip_vs_unbind_scheduler(svc);
	ip_vs_scheduler_put(old_sched);

	/* Unbind persistence engine */
	old_pe = svc->pe;
	ip_vs_unbind_pe(svc);
	ip_vs_pe_put(old_pe);

	/* Unbind app inc */
	if (svc->inc) {
		ip_vs_app_inc_put(svc->inc);
		svc->inc = NULL;
	}

	/*
	 *    Unlink the whole destination list
	 */
	list_for_each_entry_safe(dest, nxt, &svc->destinations, n_list) {
		__ip_vs_unlink_dest(svc, dest, 0);
		__ip_vs_del_dest(svc->net, dest);
	}

	/*
	 *    Update the virtual service counters
	 */
	if (svc->port == FTPPORT)
		atomic_dec(&ipvs->ftpsvc_counter);
	else if (svc->port == 0)
		atomic_dec(&ipvs->nullsvc_counter);

	/*
	 *    Free the service if nobody refers to it
	 */
	if (atomic_read(&svc->refcnt) == 0) {
		IP_VS_DBG_BUF(3, "Removing service %u/%s:%u usecnt=%d\n",
			      svc->fwmark,
			      IP_VS_DBG_ADDR(svc->af, &svc->addr),
			      ntohs(svc->port), atomic_read(&svc->usecnt));
		free_percpu(svc->stats.cpustats);
		kfree(svc);
	}

	/* decrease the module use count */
	ip_vs_use_count_dec();
}
