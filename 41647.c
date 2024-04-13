static int sctp_getsockopt_maxseg(struct sock *sk, int len,
				  char __user *optval, int __user *optlen)
{
	struct sctp_assoc_value params;
	struct sctp_association *asoc;

	if (len == sizeof(int)) {
		pr_warn("Use of int in maxseg socket option deprecated\n");
		pr_warn("Use struct sctp_assoc_value instead\n");
		params.assoc_id = 0;
	} else if (len >= sizeof(struct sctp_assoc_value)) {
		len = sizeof(struct sctp_assoc_value);
		if (copy_from_user(&params, optval, sizeof(params)))
			return -EFAULT;
	} else
		return -EINVAL;

	asoc = sctp_id2assoc(sk, params.assoc_id);
	if (!asoc && params.assoc_id && sctp_style(sk, UDP))
		return -EINVAL;

	if (asoc)
		params.assoc_value = asoc->frag_point;
	else
		params.assoc_value = sctp_sk(sk)->user_frag;

	if (put_user(len, optlen))
		return -EFAULT;
	if (len == sizeof(int)) {
		if (copy_to_user(optval, &params.assoc_value, len))
			return -EFAULT;
	} else {
		if (copy_to_user(optval, &params, len))
			return -EFAULT;
	}

	return 0;
}
