cifs_find_tcp_session(struct sockaddr *addr, struct smb_vol *vol)
{
	struct TCP_Server_Info *server;

	write_lock(&cifs_tcp_ses_lock);
	list_for_each_entry(server, &cifs_tcp_ses_list, tcp_ses_list) {
		/*
		 * the demux thread can exit on its own while still in CifsNew
		 * so don't accept any sockets in that state. Since the
		 * tcpStatus never changes back to CifsNew it's safe to check
		 * for this without a lock.
		 */
		if (server->tcpStatus == CifsNew)
			continue;

		if (!match_address(server, addr))
			continue;

		if (!match_security(server, vol))
			continue;

		++server->srv_count;
		write_unlock(&cifs_tcp_ses_lock);
		cFYI(1, "Existing tcp session with server found");
		return server;
	}
	write_unlock(&cifs_tcp_ses_lock);
	return NULL;
}
