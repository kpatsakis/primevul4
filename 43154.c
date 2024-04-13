smb2_async_readv(struct cifs_readdata *rdata)
{
	int rc, flags = 0;
	struct smb2_hdr *buf;
	struct cifs_io_parms io_parms;
	struct smb_rqst rqst = { .rq_iov = &rdata->iov,
				 .rq_nvec = 1 };
	struct TCP_Server_Info *server;

	cifs_dbg(FYI, "%s: offset=%llu bytes=%u\n",
		 __func__, rdata->offset, rdata->bytes);

	io_parms.tcon = tlink_tcon(rdata->cfile->tlink);
	io_parms.offset = rdata->offset;
	io_parms.length = rdata->bytes;
	io_parms.persistent_fid = rdata->cfile->fid.persistent_fid;
	io_parms.volatile_fid = rdata->cfile->fid.volatile_fid;
	io_parms.pid = rdata->pid;

	server = io_parms.tcon->ses->server;

	rc = smb2_new_read_req(&rdata->iov, &io_parms, 0, 0);
	if (rc) {
		if (rc == -EAGAIN && rdata->credits) {
			/* credits was reset by reconnect */
			rdata->credits = 0;
			/* reduce in_flight value since we won't send the req */
			spin_lock(&server->req_lock);
			server->in_flight--;
			spin_unlock(&server->req_lock);
		}
		return rc;
	}

	buf = (struct smb2_hdr *)rdata->iov.iov_base;
	/* 4 for rfc1002 length field */
	rdata->iov.iov_len = get_rfc1002_length(rdata->iov.iov_base) + 4;

	if (rdata->credits) {
		buf->CreditCharge = cpu_to_le16(DIV_ROUND_UP(rdata->bytes,
						SMB2_MAX_BUFFER_SIZE));
		spin_lock(&server->req_lock);
		server->credits += rdata->credits -
						le16_to_cpu(buf->CreditCharge);
		spin_unlock(&server->req_lock);
		wake_up(&server->request_q);
		flags = CIFS_HAS_CREDITS;
	}

	kref_get(&rdata->refcount);
	rc = cifs_call_async(io_parms.tcon->ses->server, &rqst,
			     cifs_readv_receive, smb2_readv_callback,
			     rdata, flags);
	if (rc) {
		kref_put(&rdata->refcount, cifs_readdata_release);
		cifs_stats_fail_inc(io_parms.tcon, SMB2_READ_HE);
	}

	cifs_small_buf_release(buf);
	return rc;
}
