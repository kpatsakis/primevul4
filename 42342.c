static int hci_sock_ioctl(struct socket *sock, unsigned int cmd,
			  unsigned long arg)
{
	struct sock *sk = sock->sk;
	void __user *argp = (void __user *) arg;
	int err;

	BT_DBG("cmd %x arg %lx", cmd, arg);

	switch (cmd) {
	case HCIGETDEVLIST:
		return hci_get_dev_list(argp);

	case HCIGETDEVINFO:
		return hci_get_dev_info(argp);

	case HCIGETCONNLIST:
		return hci_get_conn_list(argp);

	case HCIDEVUP:
		if (!capable(CAP_NET_ADMIN))
			return -EACCES;
		return hci_dev_open(arg);

	case HCIDEVDOWN:
		if (!capable(CAP_NET_ADMIN))
			return -EACCES;
		return hci_dev_close(arg);

	case HCIDEVRESET:
		if (!capable(CAP_NET_ADMIN))
			return -EACCES;
		return hci_dev_reset(arg);

	case HCIDEVRESTAT:
		if (!capable(CAP_NET_ADMIN))
			return -EACCES;
		return hci_dev_reset_stat(arg);

	case HCISETSCAN:
	case HCISETAUTH:
	case HCISETENCRYPT:
	case HCISETPTYPE:
	case HCISETLINKPOL:
	case HCISETLINKMODE:
	case HCISETACLMTU:
	case HCISETSCOMTU:
		if (!capable(CAP_NET_ADMIN))
			return -EACCES;
		return hci_dev_cmd(cmd, argp);

	case HCIINQUIRY:
		return hci_inquiry(argp);

	default:
		lock_sock(sk);
		err = hci_sock_bound_ioctl(sk, cmd, arg);
		release_sock(sk);
		return err;
	}
}
