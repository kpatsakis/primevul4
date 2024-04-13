static int sctp_setsockopt_paddr_thresholds(struct sock *sk,
					    char __user *optval,
					    unsigned int optlen)
{
	struct sctp_paddrthlds val;
	struct sctp_transport *trans;
	struct sctp_association *asoc;

	if (optlen < sizeof(struct sctp_paddrthlds))
		return -EINVAL;
	if (copy_from_user(&val, (struct sctp_paddrthlds __user *)optval,
			   sizeof(struct sctp_paddrthlds)))
		return -EFAULT;


	if (sctp_is_any(sk, (const union sctp_addr *)&val.spt_address)) {
		asoc = sctp_id2assoc(sk, val.spt_assoc_id);
		if (!asoc)
			return -ENOENT;
		list_for_each_entry(trans, &asoc->peer.transport_addr_list,
				    transports) {
			if (val.spt_pathmaxrxt)
				trans->pathmaxrxt = val.spt_pathmaxrxt;
			trans->pf_retrans = val.spt_pathpfthld;
		}

		if (val.spt_pathmaxrxt)
			asoc->pathmaxrxt = val.spt_pathmaxrxt;
		asoc->pf_retrans = val.spt_pathpfthld;
	} else {
		trans = sctp_addr_id2transport(sk, &val.spt_address,
					       val.spt_assoc_id);
		if (!trans)
			return -ENOENT;

		if (val.spt_pathmaxrxt)
			trans->pathmaxrxt = val.spt_pathmaxrxt;
		trans->pf_retrans = val.spt_pathpfthld;
	}

	return 0;
}
