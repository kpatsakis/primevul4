static inline void cleanup_sctp_mibs(struct net *net)
{
	free_percpu(net->sctp.sctp_statistics);
}
