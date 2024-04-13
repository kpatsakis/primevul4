static int sock_diag_rcv_msg(struct sk_buff *skb, struct nlmsghdr *nlh)
{
	int ret;

	switch (nlh->nlmsg_type) {
	case TCPDIAG_GETSOCK:
	case DCCPDIAG_GETSOCK:
		if (inet_rcv_compat == NULL)
			request_module("net-pf-%d-proto-%d-type-%d", PF_NETLINK,
					NETLINK_SOCK_DIAG, AF_INET);

		mutex_lock(&sock_diag_table_mutex);
		if (inet_rcv_compat != NULL)
			ret = inet_rcv_compat(skb, nlh);
		else
			ret = -EOPNOTSUPP;
		mutex_unlock(&sock_diag_table_mutex);

		return ret;
	case SOCK_DIAG_BY_FAMILY:
		return __sock_diag_rcv_msg(skb, nlh);
	default:
		return -EINVAL;
	}
}
