set_interface_linkmtu(const char *iface, uint32_t mtu)
{
	if (privsep_enabled())
		return privsep_interface_linkmtu(iface, mtu);

	return set_interface_var(iface,
				 PROC_SYS_IP6_LINKMTU, "LinkMTU",
				 mtu);
}
