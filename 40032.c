static int rfcomm_sock_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	struct sock *sk __maybe_unused = sock->sk;
	int err;

	BT_DBG("sk %p cmd %x arg %lx", sk, cmd, arg);

	err = bt_sock_ioctl(sock, cmd, arg);

	if (err == -ENOIOCTLCMD) {
#ifdef CONFIG_BT_RFCOMM_TTY
		lock_sock(sk);
		err = rfcomm_dev_ioctl(sk, cmd, (void __user *) arg);
		release_sock(sk);
#else
		err = -EOPNOTSUPP;
#endif
	}

	return err;
}
