static int sctp_setsockopt_mappedv4(struct sock *sk, char __user *optval, unsigned int optlen)
{
	int val;
	struct sctp_sock *sp = sctp_sk(sk);

	if (optlen < sizeof(int))
		return -EINVAL;
	if (get_user(val, (int __user *)optval))
		return -EFAULT;
	if (val)
		sp->v4mapped = 1;
	else
		sp->v4mapped = 0;

	return 0;
}
