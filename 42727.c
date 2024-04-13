cifs_put_tcp_session(struct TCP_Server_Info *server)
{
	struct task_struct *task;

	write_lock(&cifs_tcp_ses_lock);
	if (--server->srv_count > 0) {
		write_unlock(&cifs_tcp_ses_lock);
		return;
	}

	list_del_init(&server->tcp_ses_list);
	write_unlock(&cifs_tcp_ses_lock);

	spin_lock(&GlobalMid_Lock);
	server->tcpStatus = CifsExiting;
	spin_unlock(&GlobalMid_Lock);

	cifs_fscache_release_client_cookie(server);

	task = xchg(&server->tsk, NULL);
	if (task)
		force_sig(SIGKILL, task);
}
