static void sock_diag_rcv(struct sk_buff *skb)
{
	mutex_lock(&sock_diag_mutex);
	netlink_rcv_skb(skb, &sock_diag_rcv_msg);
	mutex_unlock(&sock_diag_mutex);
}
