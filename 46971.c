struct sctp_af *sctp_get_af_specific(sa_family_t family)
{
	switch (family) {
	case AF_INET:
		return sctp_af_v4_specific;
	case AF_INET6:
		return sctp_af_v6_specific;
	default:
		return NULL;
	}
}
