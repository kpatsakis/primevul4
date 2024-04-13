static int sctp_setsockopt_autoclose(struct sock *sk, char __user *optval,
					    int optlen)
{
	struct sctp_sock *sp = sctp_sk(sk);

	/* Applicable to UDP-style socket only */
	if (sctp_style(sk, TCP))
		return -EOPNOTSUPP;
	if (optlen != sizeof(int))
		return -EINVAL;
	if (copy_from_user(&sp->autoclose, optval, optlen))
		return -EFAULT;

	return 0;
}
