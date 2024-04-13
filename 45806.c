static int x25_setsockopt(struct socket *sock, int level, int optname,
			  char __user *optval, unsigned int optlen)
{
	int opt;
	struct sock *sk = sock->sk;
	int rc = -ENOPROTOOPT;

	if (level != SOL_X25 || optname != X25_QBITINCL)
		goto out;

	rc = -EINVAL;
	if (optlen < sizeof(int))
		goto out;

	rc = -EFAULT;
	if (get_user(opt, (int __user *)optval))
		goto out;

	if (opt)
		set_bit(X25_Q_BIT_FLAG, &x25_sk(sk)->flags);
	else
		clear_bit(X25_Q_BIT_FLAG, &x25_sk(sk)->flags);
	rc = 0;
out:
	return rc;
}
