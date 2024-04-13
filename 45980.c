set_interface_retranstimer(const char *iface, uint32_t rettimer)
{
	int ret;

	if (privsep_enabled())
		return privsep_interface_retranstimer(iface, rettimer);

	ret = set_interface_var(iface,
				PROC_SYS_IP6_RETRANSTIMER_MS,
				NULL,
				rettimer);
	if (ret)
		ret = set_interface_var(iface,
					PROC_SYS_IP6_RETRANSTIMER,
					"RetransTimer",
					rettimer / 1000 * USER_HZ); /* XXX user_hz */
	return ret;
}
