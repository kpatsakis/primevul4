static int sctp_setsockopt_reset_assoc(struct sock *sk,
				       char __user *optval,
				       unsigned int optlen)
{
	struct sctp_association *asoc;
	sctp_assoc_t associd;
	int retval = -EINVAL;

	if (optlen != sizeof(associd))
		goto out;

	if (copy_from_user(&associd, optval, optlen)) {
		retval = -EFAULT;
		goto out;
	}

	asoc = sctp_id2assoc(sk, associd);
	if (!asoc)
		goto out;

	retval = sctp_send_reset_assoc(asoc);

out:
	return retval;
}
