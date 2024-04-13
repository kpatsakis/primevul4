void sock_diag_unregister_inet_compat(int (*fn)(struct sk_buff *skb, struct nlmsghdr *nlh))
{
	mutex_lock(&sock_diag_table_mutex);
	inet_rcv_compat = NULL;
	mutex_unlock(&sock_diag_table_mutex);
}
