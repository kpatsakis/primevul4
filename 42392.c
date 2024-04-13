ip_vs_add_service(struct net *net, struct ip_vs_service_user_kern *u,
		  struct ip_vs_service **svc_p)
{
	int ret = 0;
	struct ip_vs_scheduler *sched = NULL;
	struct ip_vs_pe *pe = NULL;
	struct ip_vs_service *svc = NULL;
	struct netns_ipvs *ipvs = net_ipvs(net);

	/* increase the module use count */
	ip_vs_use_count_inc();

	/* Lookup the scheduler by 'u->sched_name' */
	sched = ip_vs_scheduler_get(u->sched_name);
	if (sched == NULL) {
		pr_info("Scheduler module ip_vs_%s not found\n", u->sched_name);
		ret = -ENOENT;
		goto out_err;
	}

	if (u->pe_name && *u->pe_name) {
		pe = ip_vs_pe_getbyname(u->pe_name);
		if (pe == NULL) {
			pr_info("persistence engine module ip_vs_pe_%s "
				"not found\n", u->pe_name);
			ret = -ENOENT;
			goto out_err;
		}
	}

#ifdef CONFIG_IP_VS_IPV6
	if (u->af == AF_INET6 && (u->netmask < 1 || u->netmask > 128)) {
		ret = -EINVAL;
		goto out_err;
	}
#endif

	svc = kzalloc(sizeof(struct ip_vs_service), GFP_KERNEL);
	if (svc == NULL) {
		IP_VS_DBG(1, "%s(): no memory\n", __func__);
		ret = -ENOMEM;
		goto out_err;
	}
	svc->stats.cpustats = alloc_percpu(struct ip_vs_cpu_stats);
	if (!svc->stats.cpustats)
		goto out_err;

	/* I'm the first user of the service */
	atomic_set(&svc->usecnt, 0);
	atomic_set(&svc->refcnt, 0);

	svc->af = u->af;
	svc->protocol = u->protocol;
	ip_vs_addr_copy(svc->af, &svc->addr, &u->addr);
	svc->port = u->port;
	svc->fwmark = u->fwmark;
	svc->flags = u->flags;
	svc->timeout = u->timeout * HZ;
	svc->netmask = u->netmask;
	svc->net = net;

	INIT_LIST_HEAD(&svc->destinations);
	rwlock_init(&svc->sched_lock);
	spin_lock_init(&svc->stats.lock);

	/* Bind the scheduler */
	ret = ip_vs_bind_scheduler(svc, sched);
	if (ret)
		goto out_err;
	sched = NULL;

	/* Bind the ct retriever */
	ip_vs_bind_pe(svc, pe);
	pe = NULL;

	/* Update the virtual service counters */
	if (svc->port == FTPPORT)
		atomic_inc(&ipvs->ftpsvc_counter);
	else if (svc->port == 0)
		atomic_inc(&ipvs->nullsvc_counter);

	ip_vs_start_estimator(net, &svc->stats);

	/* Count only IPv4 services for old get/setsockopt interface */
	if (svc->af == AF_INET)
		ipvs->num_services++;

	/* Hash the service into the service table */
	write_lock_bh(&__ip_vs_svc_lock);
	ip_vs_svc_hash(svc);
	write_unlock_bh(&__ip_vs_svc_lock);

	*svc_p = svc;
	/* Now there is a service - full throttle */
	ipvs->enable = 1;
	return 0;


 out_err:
	if (svc != NULL) {
		ip_vs_unbind_scheduler(svc);
		if (svc->inc) {
			local_bh_disable();
			ip_vs_app_inc_put(svc->inc);
			local_bh_enable();
		}
		if (svc->stats.cpustats)
			free_percpu(svc->stats.cpustats);
		kfree(svc);
	}
	ip_vs_scheduler_put(sched);
	ip_vs_pe_put(pe);

	/* decrease the module use count */
	ip_vs_use_count_dec();

	return ret;
}
