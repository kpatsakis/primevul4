static int nfs_do_multiple_writes(struct list_head *head,
		const struct rpc_call_ops *call_ops,
		int how)
{
	struct nfs_write_data *data;
	int ret = 0;

	while (!list_empty(head)) {
		int ret2;

		data = list_first_entry(head, struct nfs_write_data, list);
		list_del_init(&data->list);
		
		ret2 = nfs_do_write(data, call_ops, how);
		 if (ret == 0)
			 ret = ret2;
	}
	return ret;
}
