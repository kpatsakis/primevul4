int __init isi_register(void)
{
	return phonet_proto_register(PN_PROTO_PHONET, &pn_dgram_proto);
}
