static int ip_vs_stats_show(struct seq_file *seq, void *v)
{
	struct net *net = seq_file_single_net(seq);
	struct ip_vs_stats_user show;

/*               01234567 01234567 01234567 0123456701234567 0123456701234567 */
	seq_puts(seq,
		 "   Total Incoming Outgoing         Incoming         Outgoing\n");
	seq_printf(seq,
		   "   Conns  Packets  Packets            Bytes            Bytes\n");

	ip_vs_copy_stats(&show, &net_ipvs(net)->tot_stats);
	seq_printf(seq, "%8X %8X %8X %16LX %16LX\n\n", show.conns,
		   show.inpkts, show.outpkts,
		   (unsigned long long) show.inbytes,
		   (unsigned long long) show.outbytes);

/*                 01234567 01234567 01234567 0123456701234567 0123456701234567 */
	seq_puts(seq,
		   " Conns/s   Pkts/s   Pkts/s          Bytes/s          Bytes/s\n");
	seq_printf(seq, "%8X %8X %8X %16X %16X\n",
			show.cps, show.inpps, show.outpps,
			show.inbps, show.outbps);

	return 0;
}
