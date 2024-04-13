static bool setsockopt_needs_rtnl(int optname)
{
	switch (optname) {
	case IP_ADD_MEMBERSHIP:
	case IP_ADD_SOURCE_MEMBERSHIP:
	case IP_BLOCK_SOURCE:
	case IP_DROP_MEMBERSHIP:
	case IP_DROP_SOURCE_MEMBERSHIP:
	case IP_MSFILTER:
	case IP_UNBLOCK_SOURCE:
	case MCAST_BLOCK_SOURCE:
	case MCAST_MSFILTER:
	case MCAST_JOIN_GROUP:
	case MCAST_JOIN_SOURCE_GROUP:
	case MCAST_LEAVE_GROUP:
	case MCAST_LEAVE_SOURCE_GROUP:
	case MCAST_UNBLOCK_SOURCE:
		return true;
	}
	return false;
}
