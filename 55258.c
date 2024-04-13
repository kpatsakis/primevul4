static int sctp_setsockopt_enable_strreset(struct sock *sk,
					   char __user *optval,
					   unsigned int optlen)
{
	struct sctp_assoc_value params;
	struct sctp_association *asoc;
	int retval = -EINVAL;

	if (optlen != sizeof(params))
		goto out;

	if (copy_from_user(&params, optval, optlen)) {
		retval = -EFAULT;
		goto out;
	}

	if (params.assoc_value & (~SCTP_ENABLE_STRRESET_MASK))
		goto out;

	asoc = sctp_id2assoc(sk, params.assoc_id);
	if (asoc) {
		asoc->strreset_enable = params.assoc_value;
	} else if (!params.assoc_id) {
		struct sctp_sock *sp = sctp_sk(sk);

		sp->ep->strreset_enable = params.assoc_value;
	} else {
		goto out;
	}

	retval = 0;

out:
	return retval;
}
