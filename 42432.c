void ip_vs_service_net_cleanup(struct net *net)
{
	EnterFunction(2);
	/* Check for "full" addressed entries */
	mutex_lock(&__ip_vs_mutex);
	ip_vs_flush(net);
	mutex_unlock(&__ip_vs_mutex);
	LeaveFunction(2);
}
