static int sctp_setsockopt_add_streams(struct sock *sk,
				       char __user *optval,
				       unsigned int optlen)
{
	struct sctp_association *asoc;
	struct sctp_add_streams params;
	int retval = -EINVAL;

	if (optlen != sizeof(params))
		goto out;

	if (copy_from_user(&params, optval, optlen)) {
		retval = -EFAULT;
		goto out;
	}

	asoc = sctp_id2assoc(sk, params.sas_assoc_id);
	if (!asoc)
		goto out;

	retval = sctp_send_add_streams(asoc, &params);

out:
	return retval;
}
