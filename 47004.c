static void sctp_v4_pf_init(void)
{
	/* Initialize the SCTP specific PF functions. */
	sctp_register_pf(&sctp_pf_inet, PF_INET);
	sctp_register_af(&sctp_af_inet);
}
