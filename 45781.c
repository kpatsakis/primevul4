static int compat_x25_ioctl(struct socket *sock, unsigned int cmd,
				unsigned long arg)
{
	void __user *argp = compat_ptr(arg);
	struct sock *sk = sock->sk;

	int rc = -ENOIOCTLCMD;

	switch(cmd) {
	case TIOCOUTQ:
	case TIOCINQ:
		rc = x25_ioctl(sock, cmd, (unsigned long)argp);
		break;
	case SIOCGSTAMP:
		rc = -EINVAL;
		if (sk)
			rc = compat_sock_get_timestamp(sk,
					(struct timeval __user*)argp);
		break;
	case SIOCGSTAMPNS:
		rc = -EINVAL;
		if (sk)
			rc = compat_sock_get_timestampns(sk,
					(struct timespec __user*)argp);
		break;
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
		rc = -EINVAL;
		break;
	case SIOCADDRT:
	case SIOCDELRT:
		rc = -EPERM;
		if (!capable(CAP_NET_ADMIN))
			break;
		rc = x25_route_ioctl(cmd, argp);
		break;
	case SIOCX25GSUBSCRIP:
		rc = compat_x25_subscr_ioctl(cmd, argp);
		break;
	case SIOCX25SSUBSCRIP:
		rc = -EPERM;
		if (!capable(CAP_NET_ADMIN))
			break;
		rc = compat_x25_subscr_ioctl(cmd, argp);
		break;
	case SIOCX25GFACILITIES:
	case SIOCX25SFACILITIES:
	case SIOCX25GDTEFACILITIES:
	case SIOCX25SDTEFACILITIES:
	case SIOCX25GCALLUSERDATA:
	case SIOCX25SCALLUSERDATA:
	case SIOCX25GCAUSEDIAG:
	case SIOCX25SCAUSEDIAG:
	case SIOCX25SCUDMATCHLEN:
	case SIOCX25CALLACCPTAPPRV:
	case SIOCX25SENDCALLACCPT:
		rc = x25_ioctl(sock, cmd, (unsigned long)argp);
		break;
	default:
		rc = -ENOIOCTLCMD;
		break;
	}
	return rc;
}
