static int atalk_compat_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	/*
	 * SIOCATALKDIFADDR is a SIOCPROTOPRIVATE ioctl number, so we
	 * cannot handle it in common code. The data we access if ifreq
	 * here is compatible, so we can simply call the native
	 * handler.
	 */
	if (cmd == SIOCATALKDIFADDR)
		return atalk_ioctl(sock, cmd, (unsigned long)compat_ptr(arg));

	return -ENOIOCTLCMD;
}
