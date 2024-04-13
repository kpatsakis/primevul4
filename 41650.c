static int sctp_getsockopt_partial_delivery_point(struct sock *sk, int len,
						  char __user *optval,
						  int __user *optlen)
{
	u32 val;

	if (len < sizeof(u32))
		return -EINVAL;

	len = sizeof(u32);

	val = sctp_sk(sk)->pd_point;
	if (put_user(len, optlen))
		return -EFAULT;
	if (copy_to_user(optval, &val, len))
		return -EFAULT;

	return 0;
}
