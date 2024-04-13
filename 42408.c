static int ip_vs_flush(struct net *net)
{
	int idx;
	struct ip_vs_service *svc, *nxt;

	/*
	 * Flush the service table hashed by <netns,protocol,addr,port>
	 */
	for(idx = 0; idx < IP_VS_SVC_TAB_SIZE; idx++) {
		list_for_each_entry_safe(svc, nxt, &ip_vs_svc_table[idx],
					 s_list) {
			if (net_eq(svc->net, net))
				ip_vs_unlink_service(svc);
		}
	}

	/*
	 * Flush the service table hashed by fwmark
	 */
	for(idx = 0; idx < IP_VS_SVC_TAB_SIZE; idx++) {
		list_for_each_entry_safe(svc, nxt,
					 &ip_vs_svc_fwm_table[idx], f_list) {
			if (net_eq(svc->net, net))
				ip_vs_unlink_service(svc);
		}
	}

	return 0;
}
