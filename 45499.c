static int ipx_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	int rc = 0;
	long amount = 0;
	struct sock *sk = sock->sk;
	void __user *argp = (void __user *)arg;

	lock_sock(sk);
	switch (cmd) {
	case TIOCOUTQ:
		amount = sk->sk_sndbuf - sk_wmem_alloc_get(sk);
		if (amount < 0)
			amount = 0;
		rc = put_user(amount, (int __user *)argp);
		break;
	case TIOCINQ: {
		struct sk_buff *skb = skb_peek(&sk->sk_receive_queue);
		/* These two are safe on a single CPU system as only
		 * user tasks fiddle here */
		if (skb)
			amount = skb->len - sizeof(struct ipxhdr);
		rc = put_user(amount, (int __user *)argp);
		break;
	}
	case SIOCADDRT:
	case SIOCDELRT:
		rc = -EPERM;
		if (capable(CAP_NET_ADMIN))
			rc = ipxrtr_ioctl(cmd, argp);
		break;
	case SIOCSIFADDR:
	case SIOCAIPXITFCRT:
	case SIOCAIPXPRISLT:
		rc = -EPERM;
		if (!capable(CAP_NET_ADMIN))
			break;
	case SIOCGIFADDR:
		rc = ipxitf_ioctl(cmd, argp);
		break;
	case SIOCIPXCFGDATA:
		rc = ipxcfg_get_config_data(argp);
		break;
	case SIOCIPXNCPCONN:
		/*
		 * This socket wants to take care of the NCP connection
		 * handed to us in arg.
		 */
		rc = -EPERM;
		if (!capable(CAP_NET_ADMIN))
			break;
		rc = get_user(ipx_sk(sk)->ipx_ncp_conn,
			      (const unsigned short __user *)argp);
		break;
	case SIOCGSTAMP:
		rc = sock_get_timestamp(sk, argp);
		break;
	case SIOCGIFDSTADDR:
	case SIOCSIFDSTADDR:
	case SIOCGIFBRDADDR:
	case SIOCSIFBRDADDR:
	case SIOCGIFNETMASK:
	case SIOCSIFNETMASK:
		rc = -EINVAL;
		break;
	default:
		rc = -ENOIOCTLCMD;
		break;
	}
	release_sock(sk);

	return rc;
}
