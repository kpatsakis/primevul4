static void __ip_vs_del_dest(struct net *net, struct ip_vs_dest *dest)
{
	struct netns_ipvs *ipvs = net_ipvs(net);

	ip_vs_stop_estimator(net, &dest->stats);

	/*
	 *  Remove it from the d-linked list with the real services.
	 */
	write_lock_bh(&ipvs->rs_lock);
	ip_vs_rs_unhash(dest);
	write_unlock_bh(&ipvs->rs_lock);

	/*
	 *  Decrease the refcnt of the dest, and free the dest
	 *  if nobody refers to it (refcnt=0). Otherwise, throw
	 *  the destination into the trash.
	 */
	if (atomic_dec_and_test(&dest->refcnt)) {
		IP_VS_DBG_BUF(3, "Removing destination %u/%s:%u\n",
			      dest->vfwmark,
			      IP_VS_DBG_ADDR(dest->af, &dest->addr),
			      ntohs(dest->port));
		ip_vs_dst_reset(dest);
		/* simply decrease svc->refcnt here, let the caller check
		   and release the service if nobody refers to it.
		   Only user context can release destination and service,
		   and only one user context can update virtual service at a
		   time, so the operation here is OK */
		atomic_dec(&dest->svc->refcnt);
		free_percpu(dest->stats.cpustats);
		kfree(dest);
	} else {
		IP_VS_DBG_BUF(3, "Moving dest %s:%u into trash, "
			      "dest->refcnt=%d\n",
			      IP_VS_DBG_ADDR(dest->af, &dest->addr),
			      ntohs(dest->port),
			      atomic_read(&dest->refcnt));
		list_add(&dest->n_list, &ipvs->dest_trash);
		atomic_inc(&dest->refcnt);
	}
}
