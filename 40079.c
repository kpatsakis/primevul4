static int ax25_getsockopt(struct socket *sock, int level, int optname,
	char __user *optval, int __user *optlen)
{
	struct sock *sk = sock->sk;
	ax25_cb *ax25;
	struct ax25_dev *ax25_dev;
	char devname[IFNAMSIZ];
	void *valptr;
	int val = 0;
	int maxlen, length;

	if (level != SOL_AX25)
		return -ENOPROTOOPT;

	if (get_user(maxlen, optlen))
		return -EFAULT;

	if (maxlen < 1)
		return -EFAULT;

	valptr = (void *) &val;
	length = min_t(unsigned int, maxlen, sizeof(int));

	lock_sock(sk);
	ax25 = ax25_sk(sk);

	switch (optname) {
	case AX25_WINDOW:
		val = ax25->window;
		break;

	case AX25_T1:
		val = ax25->t1 / HZ;
		break;

	case AX25_T2:
		val = ax25->t2 / HZ;
		break;

	case AX25_N2:
		val = ax25->n2;
		break;

	case AX25_T3:
		val = ax25->t3 / HZ;
		break;

	case AX25_IDLE:
		val = ax25->idle / (60 * HZ);
		break;

	case AX25_BACKOFF:
		val = ax25->backoff;
		break;

	case AX25_EXTSEQ:
		val = (ax25->modulus == AX25_EMODULUS);
		break;

	case AX25_PIDINCL:
		val = ax25->pidincl;
		break;

	case AX25_IAMDIGI:
		val = ax25->iamdigi;
		break;

	case AX25_PACLEN:
		val = ax25->paclen;
		break;

	case SO_BINDTODEVICE:
		ax25_dev = ax25->ax25_dev;

		if (ax25_dev != NULL && ax25_dev->dev != NULL) {
			strlcpy(devname, ax25_dev->dev->name, sizeof(devname));
			length = strlen(devname) + 1;
		} else {
			*devname = '\0';
			length = 1;
		}

		valptr = (void *) devname;
		break;

	default:
		release_sock(sk);
		return -ENOPROTOOPT;
	}
	release_sock(sk);

	if (put_user(length, optlen))
		return -EFAULT;

	return copy_to_user(optval, valptr, length) ? -EFAULT : 0;
}
