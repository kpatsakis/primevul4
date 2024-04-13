static inline int rtm_msgindex(int msgtype)
{
	int msgindex = msgtype - RTM_BASE;

	/*
	 * msgindex < 0 implies someone tried to register a netlink
	 * control code. msgindex >= RTM_NR_MSGTYPES may indicate that
	 * the message type has not been added to linux/rtnetlink.h
	 */
	BUG_ON(msgindex < 0 || msgindex >= RTM_NR_MSGTYPES);

	return msgindex;
}
