static int ipx_compat_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	/*
	 * These 4 commands use same structure on 32bit and 64bit.  Rest of IPX
	 * commands is handled by generic ioctl code.  As these commands are
	 * SIOCPROTOPRIVATE..SIOCPROTOPRIVATE+3, they cannot be handled by generic
	 * code.
	 */
	switch (cmd) {
	case SIOCAIPXITFCRT:
	case SIOCAIPXPRISLT:
	case SIOCIPXCFGDATA:
	case SIOCIPXNCPCONN:
		return ipx_ioctl(sock, cmd, arg);
	default:
		return -ENOIOCTLCMD;
	}
}
