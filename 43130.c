SMB2_ioctl(const unsigned int xid, struct cifs_tcon *tcon, u64 persistent_fid,
	   u64 volatile_fid, u32 opcode, bool is_fsctl, char *in_data,
	   u32 indatalen, char **out_data, u32 *plen /* returned data len */)
{
	struct smb2_ioctl_req *req;
	struct smb2_ioctl_rsp *rsp;
	struct TCP_Server_Info *server;
	struct cifs_ses *ses = tcon->ses;
	struct kvec iov[2];
	int resp_buftype;
	int num_iovecs;
	int rc = 0;

	cifs_dbg(FYI, "SMB2 IOCTL\n");

	if (out_data != NULL)
		*out_data = NULL;

	/* zero out returned data len, in case of error */
	if (plen)
		*plen = 0;

	if (ses && (ses->server))
		server = ses->server;
	else
		return -EIO;

	rc = small_smb2_init(SMB2_IOCTL, tcon, (void **) &req);
	if (rc)
		return rc;

	req->CtlCode = cpu_to_le32(opcode);
	req->PersistentFileId = persistent_fid;
	req->VolatileFileId = volatile_fid;

	if (indatalen) {
		req->InputCount = cpu_to_le32(indatalen);
		/* do not set InputOffset if no input data */
		req->InputOffset =
		       cpu_to_le32(offsetof(struct smb2_ioctl_req, Buffer) - 4);
		iov[1].iov_base = in_data;
		iov[1].iov_len = indatalen;
		num_iovecs = 2;
	} else
		num_iovecs = 1;

	req->OutputOffset = 0;
	req->OutputCount = 0; /* MBZ */

	/*
	 * Could increase MaxOutputResponse, but that would require more
	 * than one credit. Windows typically sets this smaller, but for some
	 * ioctls it may be useful to allow server to send more. No point
	 * limiting what the server can send as long as fits in one credit
	 */
	req->MaxOutputResponse = cpu_to_le32(0xFF00); /* < 64K uses 1 credit */

	if (is_fsctl)
		req->Flags = cpu_to_le32(SMB2_0_IOCTL_IS_FSCTL);
	else
		req->Flags = 0;

	iov[0].iov_base = (char *)req;

	/*
	 * If no input data, the size of ioctl struct in
	 * protocol spec still includes a 1 byte data buffer,
	 * but if input data passed to ioctl, we do not
	 * want to double count this, so we do not send
	 * the dummy one byte of data in iovec[0] if sending
	 * input data (in iovec[1]). We also must add 4 bytes
	 * in first iovec to allow for rfc1002 length field.
	 */

	if (indatalen) {
		iov[0].iov_len = get_rfc1002_length(req) + 4 - 1;
		inc_rfc1001_len(req, indatalen - 1);
	} else
		iov[0].iov_len = get_rfc1002_length(req) + 4;


	rc = SendReceive2(xid, ses, iov, num_iovecs, &resp_buftype, 0);
	rsp = (struct smb2_ioctl_rsp *)iov[0].iov_base;

	if ((rc != 0) && (rc != -EINVAL)) {
		if (tcon)
			cifs_stats_fail_inc(tcon, SMB2_IOCTL_HE);
		goto ioctl_exit;
	} else if (rc == -EINVAL) {
		if ((opcode != FSCTL_SRV_COPYCHUNK_WRITE) &&
		    (opcode != FSCTL_SRV_COPYCHUNK)) {
			if (tcon)
				cifs_stats_fail_inc(tcon, SMB2_IOCTL_HE);
			goto ioctl_exit;
		}
	}

	/* check if caller wants to look at return data or just return rc */
	if ((plen == NULL) || (out_data == NULL))
		goto ioctl_exit;

	*plen = le32_to_cpu(rsp->OutputCount);

	/* We check for obvious errors in the output buffer length and offset */
	if (*plen == 0)
		goto ioctl_exit; /* server returned no data */
	else if (*plen > 0xFF00) {
		cifs_dbg(VFS, "srv returned invalid ioctl length: %d\n", *plen);
		*plen = 0;
		rc = -EIO;
		goto ioctl_exit;
	}

	if (get_rfc1002_length(rsp) < le32_to_cpu(rsp->OutputOffset) + *plen) {
		cifs_dbg(VFS, "Malformed ioctl resp: len %d offset %d\n", *plen,
			le32_to_cpu(rsp->OutputOffset));
		*plen = 0;
		rc = -EIO;
		goto ioctl_exit;
	}

	*out_data = kmalloc(*plen, GFP_KERNEL);
	if (*out_data == NULL) {
		rc = -ENOMEM;
		goto ioctl_exit;
	}

	memcpy(*out_data, rsp->hdr.ProtocolId + le32_to_cpu(rsp->OutputOffset),
	       *plen);
ioctl_exit:
	free_rsp_buf(resp_buftype, rsp);
	return rc;
}
