static int sctp_setsockopt_maxburst(struct sock *sk,
				    char __user *optval,
				    unsigned int optlen)
{
	struct sctp_assoc_value params;
	struct sctp_sock *sp;
	struct sctp_association *asoc;
	int val;
	int assoc_id = 0;

	if (optlen == sizeof(int)) {
		pr_warn("Use of int in max_burst socket option deprecated\n");
		pr_warn("Use struct sctp_assoc_value instead\n");
		if (copy_from_user(&val, optval, optlen))
			return -EFAULT;
	} else if (optlen == sizeof(struct sctp_assoc_value)) {
		if (copy_from_user(&params, optval, optlen))
			return -EFAULT;
		val = params.assoc_value;
		assoc_id = params.assoc_id;
	} else
		return -EINVAL;

	sp = sctp_sk(sk);

	if (assoc_id != 0) {
		asoc = sctp_id2assoc(sk, assoc_id);
		if (!asoc)
			return -EINVAL;
		asoc->max_burst = val;
	} else
		sp->max_burst = val;

	return 0;
}
