static void nfnl_err_deliver(struct list_head *err_list, struct sk_buff *skb)
{
	struct nfnl_err *nfnl_err, *next;

	list_for_each_entry_safe(nfnl_err, next, err_list, head) {
		netlink_ack(skb, nfnl_err->nlh, nfnl_err->err);
		nfnl_err_del(nfnl_err);
	}
}
