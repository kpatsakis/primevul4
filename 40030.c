static int rfcomm_sock_getsockopt_old(struct socket *sock, int optname, char __user *optval, int __user *optlen)
{
	struct sock *sk = sock->sk;
	struct rfcomm_conninfo cinfo;
	struct l2cap_conn *conn = l2cap_pi(sk)->chan->conn;
	int len, err = 0;
	u32 opt;

	BT_DBG("sk %p", sk);

	if (get_user(len, optlen))
		return -EFAULT;

	lock_sock(sk);

	switch (optname) {
	case RFCOMM_LM:
		switch (rfcomm_pi(sk)->sec_level) {
		case BT_SECURITY_LOW:
			opt = RFCOMM_LM_AUTH;
			break;
		case BT_SECURITY_MEDIUM:
			opt = RFCOMM_LM_AUTH | RFCOMM_LM_ENCRYPT;
			break;
		case BT_SECURITY_HIGH:
			opt = RFCOMM_LM_AUTH | RFCOMM_LM_ENCRYPT |
							RFCOMM_LM_SECURE;
			break;
		default:
			opt = 0;
			break;
		}

		if (rfcomm_pi(sk)->role_switch)
			opt |= RFCOMM_LM_MASTER;

		if (put_user(opt, (u32 __user *) optval))
			err = -EFAULT;
		break;

	case RFCOMM_CONNINFO:
		if (sk->sk_state != BT_CONNECTED &&
					!rfcomm_pi(sk)->dlc->defer_setup) {
			err = -ENOTCONN;
			break;
		}

		memset(&cinfo, 0, sizeof(cinfo));
		cinfo.hci_handle = conn->hcon->handle;
		memcpy(cinfo.dev_class, conn->hcon->dev_class, 3);

		len = min_t(unsigned int, len, sizeof(cinfo));
		if (copy_to_user(optval, (char *) &cinfo, len))
			err = -EFAULT;

		break;

	default:
		err = -ENOPROTOOPT;
		break;
	}

	release_sock(sk);
	return err;
}
