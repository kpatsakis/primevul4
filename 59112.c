static bool getsockopt_needs_rtnl(int optname)
{
	switch (optname) {
	case IP_MSFILTER:
	case MCAST_MSFILTER:
		return true;
	}
	return false;
}
