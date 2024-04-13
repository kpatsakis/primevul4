static int sctp_v4_protosw_init(void)
{
	int rc;

	rc = proto_register(&sctp_prot, 1);
	if (rc)
		return rc;

	/* Register SCTP(UDP and TCP style) with socket layer.  */
	inet_register_protosw(&sctp_seqpacket_protosw);
	inet_register_protosw(&sctp_stream_protosw);

	return 0;
}
