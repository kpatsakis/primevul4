int cifs_setup_session(unsigned int xid, struct cifsSesInfo *ses,
			struct nls_table *nls_info)
{
	int rc = 0;
	struct TCP_Server_Info *server = ses->server;

	ses->flags = 0;
	ses->capabilities = server->capabilities;
	if (linuxExtEnabled == 0)
		ses->capabilities &= (~CAP_UNIX);

	cFYI(1, "Security Mode: 0x%x Capabilities: 0x%x TimeAdjust: %d",
		 server->secMode, server->capabilities, server->timeAdj);

	rc = CIFS_SessSetup(xid, ses, nls_info);
	if (rc) {
		cERROR(1, "Send error in SessSetup = %d", rc);
	} else {
		cFYI(1, "CIFS Session Established successfully");
		spin_lock(&GlobalMid_Lock);
		ses->status = CifsGood;
		ses->need_reconnect = false;
		spin_unlock(&GlobalMid_Lock);
	}

	return rc;
}
