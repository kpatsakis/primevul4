static inline int pfkey_mode_to_xfrm(int mode)
{
	switch(mode) {
	case IPSEC_MODE_ANY:	/*XXX*/
	case IPSEC_MODE_TRANSPORT:
		return XFRM_MODE_TRANSPORT;
	case IPSEC_MODE_TUNNEL:
		return XFRM_MODE_TUNNEL;
	case IPSEC_MODE_BEET:
		return XFRM_MODE_BEET;
	default:
		return -1;
	}
}
