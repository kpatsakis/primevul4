static int x25_getsockopt(struct socket *sock, int level, int optname,
			  char __user *optval, int __user *optlen)
{
	struct sock *sk = sock->sk;
	int val, len, rc = -ENOPROTOOPT;

	if (level != SOL_X25 || optname != X25_QBITINCL)
		goto out;

	rc = -EFAULT;
	if (get_user(len, optlen))
		goto out;

	len = min_t(unsigned int, len, sizeof(int));

	rc = -EINVAL;
	if (len < 0)
		goto out;

	rc = -EFAULT;
	if (put_user(len, optlen))
		goto out;

	val = test_bit(X25_Q_BIT_FLAG, &x25_sk(sk)->flags);
	rc = copy_to_user(optval, &val, len) ? -EFAULT : 0;
out:
	return rc;
}
