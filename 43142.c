SMB2_tdis(const unsigned int xid, struct cifs_tcon *tcon)
{
	struct smb2_tree_disconnect_req *req; /* response is trivial */
	int rc = 0;
	struct TCP_Server_Info *server;
	struct cifs_ses *ses = tcon->ses;

	cifs_dbg(FYI, "Tree Disconnect\n");

	if (ses && (ses->server))
		server = ses->server;
	else
		return -EIO;

	if ((tcon->need_reconnect) || (tcon->ses->need_reconnect))
		return 0;

	rc = small_smb2_init(SMB2_TREE_DISCONNECT, tcon, (void **) &req);
	if (rc)
		return rc;

	rc = SendReceiveNoRsp(xid, ses, (char *)&req->hdr, 0);
	if (rc)
		cifs_stats_fail_inc(tcon, SMB2_TREE_DISCONNECT_HE);

	return rc;
}
