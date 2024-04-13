static void get_openreq6(struct seq_file *seq,
			 const struct request_sock *req, int i)
{
	long ttd = req->rsk_timer.expires - jiffies;
	const struct in6_addr *src = &inet_rsk(req)->ir_v6_loc_addr;
	const struct in6_addr *dest = &inet_rsk(req)->ir_v6_rmt_addr;

	if (ttd < 0)
		ttd = 0;

	seq_printf(seq,
		   "%4d: %08X%08X%08X%08X:%04X %08X%08X%08X%08X:%04X "
		   "%02X %08X:%08X %02X:%08lX %08X %5u %8d %d %d %pK\n",
		   i,
		   src->s6_addr32[0], src->s6_addr32[1],
		   src->s6_addr32[2], src->s6_addr32[3],
		   inet_rsk(req)->ir_num,
		   dest->s6_addr32[0], dest->s6_addr32[1],
		   dest->s6_addr32[2], dest->s6_addr32[3],
		   ntohs(inet_rsk(req)->ir_rmt_port),
		   TCP_SYN_RECV,
		   0, 0, /* could print option size, but that is af dependent. */
		   1,   /* timers active (only the expire timer) */
		   jiffies_to_clock_t(ttd),
		   req->num_timeout,
		   from_kuid_munged(seq_user_ns(seq),
				    sock_i_uid(req->rsk_listener)),
		   0,  /* non standard timer */
		   0, /* open_requests have no inode */
		   0, req);
}
