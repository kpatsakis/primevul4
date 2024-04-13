static int get_mtu(char *name)
{
	int idx = if_nametoindex(name);
	return netdev_get_mtu(idx);
}
