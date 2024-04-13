static void ipxitf_clear_primary_net(void)
{
	ipx_primary_net = NULL;
	if (ipxcfg_auto_select_primary)
		ipx_primary_net = ipx_interfaces_head();
}
