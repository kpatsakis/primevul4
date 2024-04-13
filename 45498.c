struct ipx_interface *ipx_interfaces_head(void)
{
	struct ipx_interface *rc = NULL;

	if (!list_empty(&ipx_interfaces))
		rc = list_entry(ipx_interfaces.next,
				struct ipx_interface, node);
	return rc;
}
