static uint8_t pfkey_proto_from_xfrm(uint8_t proto)
{
	return proto ? proto : IPSEC_PROTO_ANY;
}
