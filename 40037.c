static int rfcomm_sock_setsockopt(struct socket *sock, int level, int optname, char __user *optval, unsigned int optlen)
{
	struct sock *sk = sock->sk;
	struct bt_security sec;
	int err = 0;
	size_t len;
	u32 opt;

	BT_DBG("sk %p", sk);

	if (level == SOL_RFCOMM)
		return rfcomm_sock_setsockopt_old(sock, optname, optval, optlen);

	if (level != SOL_BLUETOOTH)
		return -ENOPROTOOPT;

	lock_sock(sk);

	switch (optname) {
	case BT_SECURITY:
		if (sk->sk_type != SOCK_STREAM) {
			err = -EINVAL;
			break;
		}

		sec.level = BT_SECURITY_LOW;

		len = min_t(unsigned int, sizeof(sec), optlen);
		if (copy_from_user((char *) &sec, optval, len)) {
			err = -EFAULT;
			break;
		}

		if (sec.level > BT_SECURITY_HIGH) {
			err = -EINVAL;
			break;
		}

		rfcomm_pi(sk)->sec_level = sec.level;
		break;

	case BT_DEFER_SETUP:
		if (sk->sk_state != BT_BOUND && sk->sk_state != BT_LISTEN) {
			err = -EINVAL;
			break;
		}

		if (get_user(opt, (u32 __user *) optval)) {
			err = -EFAULT;
			break;
		}

		if (opt)
			set_bit(BT_SK_DEFER_SETUP, &bt_sk(sk)->flags);
		else
			clear_bit(BT_SK_DEFER_SETUP, &bt_sk(sk)->flags);

		break;

	default:
		err = -ENOPROTOOPT;
		break;
	}

	release_sock(sk);
	return err;
}
