static int sctp_getsockopt_enable_strreset(struct sock *sk, int len,
					   char __user *optval,
					   int __user *optlen)
{
	struct sctp_assoc_value params;
	struct sctp_association *asoc;
	int retval = -EFAULT;

	if (len < sizeof(params)) {
		retval = -EINVAL;
		goto out;
	}

	len = sizeof(params);
	if (copy_from_user(&params, optval, len))
		goto out;

	asoc = sctp_id2assoc(sk, params.assoc_id);
	if (asoc) {
		params.assoc_value = asoc->strreset_enable;
	} else if (!params.assoc_id) {
		struct sctp_sock *sp = sctp_sk(sk);

		params.assoc_value = sp->ep->strreset_enable;
	} else {
		retval = -EINVAL;
		goto out;
	}

	if (put_user(len, optlen))
		goto out;

	if (copy_to_user(optval, &params, len))
		goto out;

	retval = 0;

out:
	return retval;
}
