SMB2_echo(struct TCP_Server_Info *server)
{
	struct smb2_echo_req *req;
	int rc = 0;
	struct kvec iov;
	struct smb_rqst rqst = { .rq_iov = &iov,
				 .rq_nvec = 1 };

	cifs_dbg(FYI, "In echo request\n");

	rc = small_smb2_init(SMB2_ECHO, NULL, (void **)&req);
	if (rc)
		return rc;

	req->hdr.CreditRequest = cpu_to_le16(1);

	iov.iov_base = (char *)req;
	/* 4 for rfc1002 length field */
	iov.iov_len = get_rfc1002_length(req) + 4;

	rc = cifs_call_async(server, &rqst, NULL, smb2_echo_callback, server,
			     CIFS_ECHO_OP);
	if (rc)
		cifs_dbg(FYI, "Echo request failed: %d\n", rc);

	cifs_small_buf_release(req);
	return rc;
}
