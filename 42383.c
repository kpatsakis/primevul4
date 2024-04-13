__ip_vs_get_dest_entries(struct net *net, const struct ip_vs_get_dests *get,
			 struct ip_vs_get_dests __user *uptr)
{
	struct ip_vs_service *svc;
	union nf_inet_addr addr = { .ip = get->addr };
	int ret = 0;

	if (get->fwmark)
		svc = __ip_vs_svc_fwm_find(net, AF_INET, get->fwmark);
	else
		svc = __ip_vs_service_find(net, AF_INET, get->protocol, &addr,
					   get->port);

	if (svc) {
		int count = 0;
		struct ip_vs_dest *dest;
		struct ip_vs_dest_entry entry;

		list_for_each_entry(dest, &svc->destinations, n_list) {
			if (count >= get->num_dests)
				break;

			entry.addr = dest->addr.ip;
			entry.port = dest->port;
			entry.conn_flags = atomic_read(&dest->conn_flags);
			entry.weight = atomic_read(&dest->weight);
			entry.u_threshold = dest->u_threshold;
			entry.l_threshold = dest->l_threshold;
			entry.activeconns = atomic_read(&dest->activeconns);
			entry.inactconns = atomic_read(&dest->inactconns);
			entry.persistconns = atomic_read(&dest->persistconns);
			ip_vs_copy_stats(&entry.stats, &dest->stats);
			if (copy_to_user(&uptr->entrytable[count],
					 &entry, sizeof(entry))) {
				ret = -EFAULT;
				break;
			}
			count++;
		}
	} else
		ret = -ESRCH;
	return ret;
}
