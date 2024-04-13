static int sctp_setsockopt_rtoinfo(struct sock *sk, char __user *optval, int optlen) {
	struct sctp_rtoinfo rtoinfo;
	struct sctp_association *asoc;

	if (optlen != sizeof (struct sctp_rtoinfo))
		return -EINVAL;

	if (copy_from_user(&rtoinfo, optval, optlen))
		return -EFAULT;

	asoc = sctp_id2assoc(sk, rtoinfo.srto_assoc_id);

	/* Set the values to the specific association */
	if (!asoc && rtoinfo.srto_assoc_id && sctp_style(sk, UDP))
		return -EINVAL;

	if (asoc) {
		if (rtoinfo.srto_initial != 0)
			asoc->rto_initial =
				msecs_to_jiffies(rtoinfo.srto_initial);
		if (rtoinfo.srto_max != 0)
			asoc->rto_max = msecs_to_jiffies(rtoinfo.srto_max);
		if (rtoinfo.srto_min != 0)
			asoc->rto_min = msecs_to_jiffies(rtoinfo.srto_min);
	} else {
		/* If there is no association or the association-id = 0
		 * set the values to the endpoint.
		 */
		struct sctp_sock *sp = sctp_sk(sk);

		if (rtoinfo.srto_initial != 0)
			sp->rtoinfo.srto_initial = rtoinfo.srto_initial;
		if (rtoinfo.srto_max != 0)
			sp->rtoinfo.srto_max = rtoinfo.srto_max;
		if (rtoinfo.srto_min != 0)
			sp->rtoinfo.srto_min = rtoinfo.srto_min;
	}

	return 0;
}
