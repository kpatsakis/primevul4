static int ipxitf_add_local_route(struct ipx_interface *intrfc)
{
	return ipxrtr_add_route(intrfc->if_netnum, intrfc, NULL);
}
