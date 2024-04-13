static int sctp_getsockopt_paddr_thresholds(struct sock *sk,
					    char __user *optval,
					    int len,
					    int __user *optlen)
{
	struct sctp_paddrthlds val;
	struct sctp_transport *trans;
	struct sctp_association *asoc;

	if (len < sizeof(struct sctp_paddrthlds))
		return -EINVAL;
	len = sizeof(struct sctp_paddrthlds);
	if (copy_from_user(&val, (struct sctp_paddrthlds __user *)optval, len))
		return -EFAULT;

	if (sctp_is_any(sk, (const union sctp_addr *)&val.spt_address)) {
		asoc = sctp_id2assoc(sk, val.spt_assoc_id);
		if (!asoc)
			return -ENOENT;

		val.spt_pathpfthld = asoc->pf_retrans;
		val.spt_pathmaxrxt = asoc->pathmaxrxt;
	} else {
		trans = sctp_addr_id2transport(sk, &val.spt_address,
					       val.spt_assoc_id);
		if (!trans)
			return -ENOENT;

		val.spt_pathmaxrxt = trans->pathmaxrxt;
		val.spt_pathpfthld = trans->pf_retrans;
	}

	if (put_user(len, optlen) || copy_to_user(optval, &val, len))
		return -EFAULT;

	return 0;
}
