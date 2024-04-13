static void ip_vs_trash_cleanup(struct net *net)
{
	struct ip_vs_dest *dest, *nxt;
	struct netns_ipvs *ipvs = net_ipvs(net);

	list_for_each_entry_safe(dest, nxt, &ipvs->dest_trash, n_list) {
		list_del(&dest->n_list);
		ip_vs_dst_reset(dest);
		__ip_vs_unbind_svc(dest);
		free_percpu(dest->stats.cpustats);
		kfree(dest);
	}
}
