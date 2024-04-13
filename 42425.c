static int ip_vs_info_seq_show(struct seq_file *seq, void *v)
{
	if (v == SEQ_START_TOKEN) {
		seq_printf(seq,
			"IP Virtual Server version %d.%d.%d (size=%d)\n",
			NVERSION(IP_VS_VERSION_CODE), ip_vs_conn_tab_size);
		seq_puts(seq,
			 "Prot LocalAddress:Port Scheduler Flags\n");
		seq_puts(seq,
			 "  -> RemoteAddress:Port Forward Weight ActiveConn InActConn\n");
	} else {
		const struct ip_vs_service *svc = v;
		const struct ip_vs_iter *iter = seq->private;
		const struct ip_vs_dest *dest;

		if (iter->table == ip_vs_svc_table) {
#ifdef CONFIG_IP_VS_IPV6
			if (svc->af == AF_INET6)
				seq_printf(seq, "%s  [%pI6]:%04X %s ",
					   ip_vs_proto_name(svc->protocol),
					   &svc->addr.in6,
					   ntohs(svc->port),
					   svc->scheduler->name);
			else
#endif
				seq_printf(seq, "%s  %08X:%04X %s %s ",
					   ip_vs_proto_name(svc->protocol),
					   ntohl(svc->addr.ip),
					   ntohs(svc->port),
					   svc->scheduler->name,
					   (svc->flags & IP_VS_SVC_F_ONEPACKET)?"ops ":"");
		} else {
			seq_printf(seq, "FWM  %08X %s %s",
				   svc->fwmark, svc->scheduler->name,
				   (svc->flags & IP_VS_SVC_F_ONEPACKET)?"ops ":"");
		}

		if (svc->flags & IP_VS_SVC_F_PERSISTENT)
			seq_printf(seq, "persistent %d %08X\n",
				svc->timeout,
				ntohl(svc->netmask));
		else
			seq_putc(seq, '\n');

		list_for_each_entry(dest, &svc->destinations, n_list) {
#ifdef CONFIG_IP_VS_IPV6
			if (dest->af == AF_INET6)
				seq_printf(seq,
					   "  -> [%pI6]:%04X"
					   "      %-7s %-6d %-10d %-10d\n",
					   &dest->addr.in6,
					   ntohs(dest->port),
					   ip_vs_fwd_name(atomic_read(&dest->conn_flags)),
					   atomic_read(&dest->weight),
					   atomic_read(&dest->activeconns),
					   atomic_read(&dest->inactconns));
			else
#endif
				seq_printf(seq,
					   "  -> %08X:%04X      "
					   "%-7s %-6d %-10d %-10d\n",
					   ntohl(dest->addr.ip),
					   ntohs(dest->port),
					   ip_vs_fwd_name(atomic_read(&dest->conn_flags)),
					   atomic_read(&dest->weight),
					   atomic_read(&dest->activeconns),
					   atomic_read(&dest->inactconns));

		}
	}
	return 0;
}
