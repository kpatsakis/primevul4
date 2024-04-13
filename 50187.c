static int sock_getbindtodevice(struct sock *sk, char __user *optval,
				int __user *optlen, int len)
{
	int ret = -ENOPROTOOPT;
#ifdef CONFIG_NETDEVICES
	struct net *net = sock_net(sk);
	char devname[IFNAMSIZ];

	if (sk->sk_bound_dev_if == 0) {
		len = 0;
		goto zero;
	}

	ret = -EINVAL;
	if (len < IFNAMSIZ)
		goto out;

	ret = netdev_get_name(net, devname, sk->sk_bound_dev_if);
	if (ret)
		goto out;

	len = strlen(devname) + 1;

	ret = -EFAULT;
	if (copy_to_user(optval, devname, len))
		goto out;

zero:
	ret = -EFAULT;
	if (put_user(len, optlen))
		goto out;

	ret = 0;

out:
#endif

	return ret;
}
