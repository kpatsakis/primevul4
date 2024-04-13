cifs_retry_async_readv(struct cifs_readdata *rdata)
{
	int rc;
	struct TCP_Server_Info *server;

	server = tlink_tcon(rdata->cfile->tlink)->ses->server;

	do {
		if (rdata->cfile->invalidHandle) {
			rc = cifs_reopen_file(rdata->cfile, true);
			if (rc != 0)
				continue;
		}
		rc = server->ops->async_readv(rdata);
	} while (rc == -EAGAIN);

	return rc;
}
