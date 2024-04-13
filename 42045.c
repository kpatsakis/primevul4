void sock_diag_unregister(const struct sock_diag_handler *hnld)
{
	int family = hnld->family;

	if (family >= AF_MAX)
		return;

	mutex_lock(&sock_diag_table_mutex);
	BUG_ON(sock_diag_handlers[family] != hnld);
	sock_diag_handlers[family] = NULL;
	mutex_unlock(&sock_diag_table_mutex);
}
