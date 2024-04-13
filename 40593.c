static inline int pfkey_mode_from_xfrm(int mode)
{
	switch(mode) {
	case XFRM_MODE_TRANSPORT:
		return IPSEC_MODE_TRANSPORT;
	case XFRM_MODE_TUNNEL:
		return IPSEC_MODE_TUNNEL;
	case XFRM_MODE_BEET:
		return IPSEC_MODE_BEET;
	default:
		return -1;
	}
}
