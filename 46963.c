static inline int init_sctp_mibs(struct net *net)
{
	net->sctp.sctp_statistics = alloc_percpu(struct sctp_mib);
	if (!net->sctp.sctp_statistics)
		return -ENOMEM;
	return 0;
}
