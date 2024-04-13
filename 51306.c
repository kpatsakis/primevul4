static int compat_sock_ioctl_trans(struct file *file, struct socket *sock,
			 unsigned int cmd, unsigned long arg)
{
	void __user *argp = compat_ptr(arg);
	struct sock *sk = sock->sk;
	struct net *net = sock_net(sk);

	if (cmd >= SIOCDEVPRIVATE && cmd <= (SIOCDEVPRIVATE + 15))
		return compat_ifr_data_ioctl(net, cmd, argp);

	switch (cmd) {
	case SIOCSIFBR:
	case SIOCGIFBR:
		return old_bridge_ioctl(argp);
	case SIOCGIFNAME:
		return dev_ifname32(net, argp);
	case SIOCGIFCONF:
		return dev_ifconf(net, argp);
	case SIOCETHTOOL:
		return ethtool_ioctl(net, argp);
	case SIOCWANDEV:
		return compat_siocwandev(net, argp);
	case SIOCGIFMAP:
	case SIOCSIFMAP:
		return compat_sioc_ifmap(net, cmd, argp);
	case SIOCBONDENSLAVE:
	case SIOCBONDRELEASE:
	case SIOCBONDSETHWADDR:
	case SIOCBONDCHANGEACTIVE:
		return bond_ioctl(net, cmd, argp);
	case SIOCADDRT:
	case SIOCDELRT:
		return routing_ioctl(net, sock, cmd, argp);
	case SIOCGSTAMP:
		return do_siocgstamp(net, sock, cmd, argp);
	case SIOCGSTAMPNS:
		return do_siocgstampns(net, sock, cmd, argp);
	case SIOCBONDSLAVEINFOQUERY:
	case SIOCBONDINFOQUERY:
	case SIOCSHWTSTAMP:
	case SIOCGHWTSTAMP:
		return compat_ifr_data_ioctl(net, cmd, argp);

	case FIOSETOWN:
	case SIOCSPGRP:
	case FIOGETOWN:
	case SIOCGPGRP:
	case SIOCBRADDBR:
	case SIOCBRDELBR:
	case SIOCGIFVLAN:
	case SIOCSIFVLAN:
	case SIOCADDDLCI:
	case SIOCDELDLCI:
		return sock_ioctl(file, cmd, arg);

	case SIOCGIFFLAGS:
	case SIOCSIFFLAGS:
	case SIOCGIFMETRIC:
	case SIOCSIFMETRIC:
	case SIOCGIFMTU:
	case SIOCSIFMTU:
	case SIOCGIFMEM:
	case SIOCSIFMEM:
	case SIOCGIFHWADDR:
	case SIOCSIFHWADDR:
	case SIOCADDMULTI:
	case SIOCDELMULTI:
	case SIOCGIFINDEX:
	case SIOCGIFADDR:
	case SIOCSIFADDR:
	case SIOCSIFHWBROADCAST:
	case SIOCDIFADDR:
	case SIOCGIFBRDADDR:
	case SIOCSIFBRDADDR:
	case SIOCGIFDSTADDR:
	case SIOCSIFDSTADDR:
	case SIOCGIFNETMASK:
	case SIOCSIFNETMASK:
	case SIOCSIFPFLAGS:
	case SIOCGIFPFLAGS:
	case SIOCGIFTXQLEN:
	case SIOCSIFTXQLEN:
	case SIOCBRADDIF:
	case SIOCBRDELIF:
	case SIOCSIFNAME:
	case SIOCGMIIPHY:
	case SIOCGMIIREG:
	case SIOCSMIIREG:
		return dev_ifsioc(net, sock, cmd, argp);

	case SIOCSARP:
	case SIOCGARP:
	case SIOCDARP:
	case SIOCATMARK:
		return sock_do_ioctl(net, sock, cmd, arg);
	}

	return -ENOIOCTLCMD;
}
