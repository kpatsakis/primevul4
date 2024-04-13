cifs_uncached_retry_writev(struct cifs_writedata *wdata)
{
	int rc;
	struct TCP_Server_Info *server;

	server = tlink_tcon(wdata->cfile->tlink)->ses->server;

	do {
		if (wdata->cfile->invalidHandle) {
			rc = cifs_reopen_file(wdata->cfile, false);
			if (rc != 0)
				continue;
		}
		rc = server->ops->async_writev(wdata,
					       cifs_uncached_writedata_release);
	} while (rc == -EAGAIN);

	return rc;
}
