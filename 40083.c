static int ax25_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	struct sock *sk = sock->sk;
	void __user *argp = (void __user *)arg;
	int res = 0;

	lock_sock(sk);
	switch (cmd) {
	case TIOCOUTQ: {
		long amount;

		amount = sk->sk_sndbuf - sk_wmem_alloc_get(sk);
		if (amount < 0)
			amount = 0;
		res = put_user(amount, (int __user *)argp);
		break;
	}

	case TIOCINQ: {
		struct sk_buff *skb;
		long amount = 0L;
		/* These two are safe on a single CPU system as only user tasks fiddle here */
		if ((skb = skb_peek(&sk->sk_receive_queue)) != NULL)
			amount = skb->len;
		res = put_user(amount, (int __user *) argp);
		break;
	}

	case SIOCGSTAMP:
		res = sock_get_timestamp(sk, argp);
		break;

	case SIOCGSTAMPNS:
		res = sock_get_timestampns(sk, argp);
		break;

	case SIOCAX25ADDUID:	/* Add a uid to the uid/call map table */
	case SIOCAX25DELUID:	/* Delete a uid from the uid/call map table */
	case SIOCAX25GETUID: {
		struct sockaddr_ax25 sax25;
		if (copy_from_user(&sax25, argp, sizeof(sax25))) {
			res = -EFAULT;
			break;
		}
		res = ax25_uid_ioctl(cmd, &sax25);
		break;
	}

	case SIOCAX25NOUID: {	/* Set the default policy (default/bar) */
		long amount;
		if (!capable(CAP_NET_ADMIN)) {
			res = -EPERM;
			break;
		}
		if (get_user(amount, (long __user *)argp)) {
			res = -EFAULT;
			break;
		}
		if (amount > AX25_NOUID_BLOCK) {
			res = -EINVAL;
			break;
		}
		ax25_uid_policy = amount;
		res = 0;
		break;
	}

	case SIOCADDRT:
	case SIOCDELRT:
	case SIOCAX25OPTRT:
		if (!capable(CAP_NET_ADMIN)) {
			res = -EPERM;
			break;
		}
		res = ax25_rt_ioctl(cmd, argp);
		break;

	case SIOCAX25CTLCON:
		if (!capable(CAP_NET_ADMIN)) {
			res = -EPERM;
			break;
		}
		res = ax25_ctl_ioctl(cmd, argp);
		break;

	case SIOCAX25GETINFO:
	case SIOCAX25GETINFOOLD: {
		ax25_cb *ax25 = ax25_sk(sk);
		struct ax25_info_struct ax25_info;

		ax25_info.t1        = ax25->t1   / HZ;
		ax25_info.t2        = ax25->t2   / HZ;
		ax25_info.t3        = ax25->t3   / HZ;
		ax25_info.idle      = ax25->idle / (60 * HZ);
		ax25_info.n2        = ax25->n2;
		ax25_info.t1timer   = ax25_display_timer(&ax25->t1timer)   / HZ;
		ax25_info.t2timer   = ax25_display_timer(&ax25->t2timer)   / HZ;
		ax25_info.t3timer   = ax25_display_timer(&ax25->t3timer)   / HZ;
		ax25_info.idletimer = ax25_display_timer(&ax25->idletimer) / (60 * HZ);
		ax25_info.n2count   = ax25->n2count;
		ax25_info.state     = ax25->state;
		ax25_info.rcv_q     = sk_rmem_alloc_get(sk);
		ax25_info.snd_q     = sk_wmem_alloc_get(sk);
		ax25_info.vs        = ax25->vs;
		ax25_info.vr        = ax25->vr;
		ax25_info.va        = ax25->va;
		ax25_info.vs_max    = ax25->vs; /* reserved */
		ax25_info.paclen    = ax25->paclen;
		ax25_info.window    = ax25->window;

		/* old structure? */
		if (cmd == SIOCAX25GETINFOOLD) {
			static int warned = 0;
			if (!warned) {
				printk(KERN_INFO "%s uses old SIOCAX25GETINFO\n",
					current->comm);
				warned=1;
			}

			if (copy_to_user(argp, &ax25_info, sizeof(struct ax25_info_struct_deprecated))) {
				res = -EFAULT;
				break;
			}
		} else {
			if (copy_to_user(argp, &ax25_info, sizeof(struct ax25_info_struct))) {
				res = -EINVAL;
				break;
			}
		}
		res = 0;
		break;
	}

	case SIOCAX25ADDFWD:
	case SIOCAX25DELFWD: {
		struct ax25_fwd_struct ax25_fwd;
		if (!capable(CAP_NET_ADMIN)) {
			res = -EPERM;
			break;
		}
		if (copy_from_user(&ax25_fwd, argp, sizeof(ax25_fwd))) {
			res = -EFAULT;
			break;
		}
		res = ax25_fwd_ioctl(cmd, &ax25_fwd);
		break;
	}

	case SIOCGIFADDR:
	case SIOCSIFADDR:
	case SIOCGIFDSTADDR:
	case SIOCSIFDSTADDR:
	case SIOCGIFBRDADDR:
	case SIOCSIFBRDADDR:
	case SIOCGIFNETMASK:
	case SIOCSIFNETMASK:
	case SIOCGIFMETRIC:
	case SIOCSIFMETRIC:
		res = -EINVAL;
		break;

	default:
		res = -ENOIOCTLCMD;
		break;
	}
	release_sock(sk);

	return res;
}
