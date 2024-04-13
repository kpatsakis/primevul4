SMB2_logoff(const unsigned int xid, struct cifs_ses *ses)
{
	struct smb2_logoff_req *req; /* response is also trivial struct */
	int rc = 0;
	struct TCP_Server_Info *server;

	cifs_dbg(FYI, "disconnect session %p\n", ses);

	if (ses && (ses->server))
		server = ses->server;
	else
		return -EIO;

	/* no need to send SMB logoff if uid already closed due to reconnect */
	if (ses->need_reconnect)
		goto smb2_session_already_dead;

	rc = small_smb2_init(SMB2_LOGOFF, NULL, (void **) &req);
	if (rc)
		return rc;

	 /* since no tcon, smb2_init can not do this, so do here */
	req->hdr.SessionId = ses->Suid;
	if (server->sign)
		req->hdr.Flags |= SMB2_FLAGS_SIGNED;

	rc = SendReceiveNoRsp(xid, ses, (char *) &req->hdr, 0);
	/*
	 * No tcon so can't do
	 * cifs_stats_inc(&tcon->stats.smb2_stats.smb2_com_fail[SMB2...]);
	 */

smb2_session_already_dead:
	return rc;
}
