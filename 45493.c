const char *ipx_device_name(struct ipx_interface *intrfc)
{
	return intrfc->if_internal ? "Internal" :
		intrfc->if_dev ? intrfc->if_dev->name : "Unknown";
}
