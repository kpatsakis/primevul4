static int sctp_getsockopt_autoclose(struct sock *sk, int len, char __user *optval, int __user *optlen)
{
	/* Applicable to UDP-style socket only */
	if (sctp_style(sk, TCP))
		return -EOPNOTSUPP;
	if (len < sizeof(int))
		return -EINVAL;
	len = sizeof(int);
	if (put_user(len, optlen))
		return -EFAULT;
	if (copy_to_user(optval, &sctp_sk(sk)->autoclose, sizeof(int)))
		return -EFAULT;
	return 0;
}
