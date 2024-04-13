static void ip_vs_unlink_service(struct ip_vs_service *svc)
{
	/*
	 * Unhash it from the service table
	 */
	write_lock_bh(&__ip_vs_svc_lock);

	ip_vs_svc_unhash(svc);

	/*
	 * Wait until all the svc users go away.
	 */
	IP_VS_WAIT_WHILE(atomic_read(&svc->usecnt) > 0);

	__ip_vs_del_service(svc);

	write_unlock_bh(&__ip_vs_svc_lock);
}
