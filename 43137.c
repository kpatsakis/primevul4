SMB2_read(const unsigned int xid, struct cifs_io_parms *io_parms,
	  unsigned int *nbytes, char **buf, int *buf_type)
{
	int resp_buftype, rc = -EACCES;
	struct smb2_read_rsp *rsp = NULL;
	struct kvec iov[1];

	*nbytes = 0;
	rc = smb2_new_read_req(iov, io_parms, 0, 0);
	if (rc)
		return rc;

	rc = SendReceive2(xid, io_parms->tcon->ses, iov, 1,
			  &resp_buftype, CIFS_LOG_ERROR);

	rsp = (struct smb2_read_rsp *)iov[0].iov_base;

	if (rsp->hdr.Status == STATUS_END_OF_FILE) {
		free_rsp_buf(resp_buftype, iov[0].iov_base);
		return 0;
	}

	if (rc) {
		cifs_stats_fail_inc(io_parms->tcon, SMB2_READ_HE);
		cifs_dbg(VFS, "Send error in read = %d\n", rc);
	} else {
		*nbytes = le32_to_cpu(rsp->DataLength);
		if ((*nbytes > CIFS_MAX_MSGSIZE) ||
		    (*nbytes > io_parms->length)) {
			cifs_dbg(FYI, "bad length %d for count %d\n",
				 *nbytes, io_parms->length);
			rc = -EIO;
			*nbytes = 0;
		}
	}

	if (*buf) {
		memcpy(*buf, (char *)rsp->hdr.ProtocolId + rsp->DataOffset,
		       *nbytes);
		free_rsp_buf(resp_buftype, iov[0].iov_base);
	} else if (resp_buftype != CIFS_NO_BUFFER) {
		*buf = iov[0].iov_base;
		if (resp_buftype == CIFS_SMALL_BUFFER)
			*buf_type = CIFS_SMALL_BUFFER;
		else if (resp_buftype == CIFS_LARGE_BUFFER)
			*buf_type = CIFS_LARGE_BUFFER;
	}
	return rc;
}
