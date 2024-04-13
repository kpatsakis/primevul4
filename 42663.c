static int sctp_getsockopt_delayed_ack_time(struct sock *sk, int len,
					    char __user *optval,
					    int __user *optlen)
{
	struct sctp_assoc_value  params;
	struct sctp_association *asoc = NULL;
	struct sctp_sock        *sp = sctp_sk(sk);

	if (len != sizeof(struct sctp_assoc_value))
		return - EINVAL;

	if (copy_from_user(&params, optval, len))
		return -EFAULT;

	/* Get association, if assoc_id != 0 and the socket is a one
	 * to many style socket, and an association was not found, then
	 * the id was invalid.
	 */
	asoc = sctp_id2assoc(sk, params.assoc_id);
	if (!asoc && params.assoc_id && sctp_style(sk, UDP))
		return -EINVAL;

	if (asoc) {
		/* Fetch association values. */
		if (asoc->param_flags & SPP_SACKDELAY_ENABLE)
			params.assoc_value = jiffies_to_msecs(
				asoc->sackdelay);
		else
			params.assoc_value = 0;
	} else {
		/* Fetch socket values. */
		if (sp->param_flags & SPP_SACKDELAY_ENABLE)
			params.assoc_value  = sp->sackdelay;
		else
			params.assoc_value  = 0;
	}

	if (copy_to_user(optval, &params, len))
		return -EFAULT;

	if (put_user(len, optlen))
		return -EFAULT;

	return 0;
}
