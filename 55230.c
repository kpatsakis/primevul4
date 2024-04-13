static int sctp_getsockopt_default_prinfo(struct sock *sk, int len,
					  char __user *optval,
					  int __user *optlen)
{
	struct sctp_default_prinfo info;
	struct sctp_association *asoc;
	int retval = -EFAULT;

	if (len < sizeof(info)) {
		retval = -EINVAL;
		goto out;
	}

	len = sizeof(info);
	if (copy_from_user(&info, optval, len))
		goto out;

	asoc = sctp_id2assoc(sk, info.pr_assoc_id);
	if (asoc) {
		info.pr_policy = SCTP_PR_POLICY(asoc->default_flags);
		info.pr_value = asoc->default_timetolive;
	} else if (!info.pr_assoc_id) {
		struct sctp_sock *sp = sctp_sk(sk);

		info.pr_policy = SCTP_PR_POLICY(sp->default_flags);
		info.pr_value = sp->default_timetolive;
	} else {
		retval = -EINVAL;
		goto out;
	}

	if (put_user(len, optlen))
		goto out;

	if (copy_to_user(optval, &info, len))
		goto out;

	retval = 0;

out:
	return retval;
}
