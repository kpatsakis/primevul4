__ip_vs_unbind_svc(struct ip_vs_dest *dest)
{
	struct ip_vs_service *svc = dest->svc;

	dest->svc = NULL;
	if (atomic_dec_and_test(&svc->refcnt)) {
		IP_VS_DBG_BUF(3, "Removing service %u/%s:%u usecnt=%d\n",
			      svc->fwmark,
			      IP_VS_DBG_ADDR(svc->af, &svc->addr),
			      ntohs(svc->port), atomic_read(&svc->usecnt));
		free_percpu(svc->stats.cpustats);
		kfree(svc);
	}
}
