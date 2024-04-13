static int sctp_getsockopt_initmsg(struct sock *sk, int len, char __user *optval, int __user *optlen)
{
	if (len < sizeof(struct sctp_initmsg))
		return -EINVAL;
	len = sizeof(struct sctp_initmsg);
	if (put_user(len, optlen))
		return -EFAULT;
	if (copy_to_user(optval, &sctp_sk(sk)->initmsg, len))
		return -EFAULT;
	return 0;
}
