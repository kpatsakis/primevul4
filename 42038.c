static const inline struct sock_diag_handler *sock_diag_lock_handler(int family)
{
	if (sock_diag_handlers[family] == NULL)
		request_module("net-pf-%d-proto-%d-type-%d", PF_NETLINK,
				NETLINK_SOCK_DIAG, family);

	mutex_lock(&sock_diag_table_mutex);
	return sock_diag_handlers[family];
}
