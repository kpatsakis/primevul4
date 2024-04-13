proto_reg_handoff_pktap(void)
{
	dissector_add_uint("wtap_encap", WTAP_ENCAP_PKTAP, pktap_handle);
}
