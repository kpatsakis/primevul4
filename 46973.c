struct sctp_pf *sctp_get_pf_specific(sa_family_t family)
{
	switch (family) {
	case PF_INET:
		return sctp_pf_inet_specific;
	case PF_INET6:
		return sctp_pf_inet6_specific;
	default:
		return NULL;
	}
}
