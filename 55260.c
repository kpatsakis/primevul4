static int sctp_setsockopt_pr_supported(struct sock *sk,
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

	asoc = sctp_id2assoc(sk, params.assoc_id);
	if (asoc) {
		asoc->prsctp_enable = !!params.assoc_value;
	} else if (!params.assoc_id) {
		struct sctp_sock *sp = sctp_sk(sk);

		sp->ep->prsctp_enable = !!params.assoc_value;
	} else {
		goto out;
	}

	retval = 0;

out:
	return retval;
}
