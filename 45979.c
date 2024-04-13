set_interface_reachtime(const char *iface, uint32_t rtime)
{
	int ret;

	if (privsep_enabled())
		return privsep_interface_reachtime(iface, rtime);

	ret = set_interface_var(iface,
				PROC_SYS_IP6_BASEREACHTIME_MS,
				NULL,
				rtime);
	if (ret)
		ret = set_interface_var(iface,
					PROC_SYS_IP6_BASEREACHTIME,
					"BaseReachableTimer",
					rtime / 1000); /* sec */
	return ret;
}
