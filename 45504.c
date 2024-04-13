static int ipx_setsockopt(struct socket *sock, int level, int optname,
			  char __user *optval, unsigned int optlen)
{
	struct sock *sk = sock->sk;
	int opt;
	int rc = -EINVAL;

	lock_sock(sk);
	if (optlen != sizeof(int))
		goto out;

	rc = -EFAULT;
	if (get_user(opt, (unsigned int __user *)optval))
		goto out;

	rc = -ENOPROTOOPT;
	if (!(level == SOL_IPX && optname == IPX_TYPE))
		goto out;

	ipx_sk(sk)->type = opt;
	rc = 0;
out:
	release_sock(sk);
	return rc;
}
