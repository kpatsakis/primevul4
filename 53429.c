static int rfcomm_sock_setsockopt_old(struct socket *sock, int optname, char __user *optval, unsigned int optlen)
{
	struct sock *sk = sock->sk;
	int err = 0;
	u32 opt;

	BT_DBG("sk %p", sk);

	lock_sock(sk);

	switch (optname) {
	case RFCOMM_LM:
		if (get_user(opt, (u32 __user *) optval)) {
			err = -EFAULT;
			break;
		}

		if (opt & RFCOMM_LM_FIPS) {
			err = -EINVAL;
			break;
		}

		if (opt & RFCOMM_LM_AUTH)
			rfcomm_pi(sk)->sec_level = BT_SECURITY_LOW;
		if (opt & RFCOMM_LM_ENCRYPT)
			rfcomm_pi(sk)->sec_level = BT_SECURITY_MEDIUM;
		if (opt & RFCOMM_LM_SECURE)
			rfcomm_pi(sk)->sec_level = BT_SECURITY_HIGH;

		rfcomm_pi(sk)->role_switch = (opt & RFCOMM_LM_MASTER);
		break;

	default:
		err = -ENOPROTOOPT;
		break;
	}

	release_sock(sk);
	return err;
}
