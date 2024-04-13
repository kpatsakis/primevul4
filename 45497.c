static int ipx_getsockopt(struct socket *sock, int level, int optname,
	char __user *optval, int __user *optlen)
{
	struct sock *sk = sock->sk;
	int val = 0;
	int len;
	int rc = -ENOPROTOOPT;

	lock_sock(sk);
	if (!(level == SOL_IPX && optname == IPX_TYPE))
		goto out;

	val = ipx_sk(sk)->type;

	rc = -EFAULT;
	if (get_user(len, optlen))
		goto out;

	len = min_t(unsigned int, len, sizeof(int));
	rc = -EINVAL;
	if(len < 0)
		goto out;

	rc = -EFAULT;
	if (put_user(len, optlen) || copy_to_user(optval, &val, len))
		goto out;

	rc = 0;
out:
	release_sock(sk);
	return rc;
}
