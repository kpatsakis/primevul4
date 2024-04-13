static int sctp_getsockopt_rtoinfo(struct sock *sk, int len,
				char __user *optval,
				int __user *optlen) {
	struct sctp_rtoinfo rtoinfo;
	struct sctp_association *asoc;

	if (len != sizeof (struct sctp_rtoinfo))
		return -EINVAL;

	if (copy_from_user(&rtoinfo, optval, sizeof (struct sctp_rtoinfo)))
		return -EFAULT;

	asoc = sctp_id2assoc(sk, rtoinfo.srto_assoc_id);

	if (!asoc && rtoinfo.srto_assoc_id && sctp_style(sk, UDP))
		return -EINVAL;

	/* Values corresponding to the specific association. */
	if (asoc) {
		rtoinfo.srto_initial = jiffies_to_msecs(asoc->rto_initial);
		rtoinfo.srto_max = jiffies_to_msecs(asoc->rto_max);
		rtoinfo.srto_min = jiffies_to_msecs(asoc->rto_min);
	} else {
		/* Values corresponding to the endpoint. */
		struct sctp_sock *sp = sctp_sk(sk);

		rtoinfo.srto_initial = sp->rtoinfo.srto_initial;
		rtoinfo.srto_max = sp->rtoinfo.srto_max;
		rtoinfo.srto_min = sp->rtoinfo.srto_min;
	}

	if (put_user(len, optlen))
		return -EFAULT;

	if (copy_to_user(optval, &rtoinfo, len))
		return -EFAULT;

	return 0;
}
