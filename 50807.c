static void nfnl_err_reset(struct list_head *err_list)
{
	struct nfnl_err *nfnl_err, *next;

	list_for_each_entry_safe(nfnl_err, next, err_list, head)
		nfnl_err_del(nfnl_err);
}
