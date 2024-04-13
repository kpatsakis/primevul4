cifs_put_smb_ses(struct cifsSesInfo *ses)
{
	int xid;
	struct TCP_Server_Info *server = ses->server;

	cFYI(1, "%s: ses_count=%d\n", __func__, ses->ses_count);
	write_lock(&cifs_tcp_ses_lock);
	if (--ses->ses_count > 0) {
		write_unlock(&cifs_tcp_ses_lock);
		return;
	}

	list_del_init(&ses->smb_ses_list);
	write_unlock(&cifs_tcp_ses_lock);

	if (ses->status == CifsGood) {
		xid = GetXid();
		CIFSSMBLogoff(xid, ses);
		_FreeXid(xid);
	}
	sesInfoFree(ses);
	cifs_put_tcp_session(server);
}
