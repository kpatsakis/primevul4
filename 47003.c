static void sctp_v4_pf_exit(void)
{
	list_del(&sctp_af_inet.list);
}
