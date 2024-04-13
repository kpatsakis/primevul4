SMB2_write(const unsigned int xid, struct cifs_io_parms *io_parms,
	   unsigned int *nbytes, struct kvec *iov, int n_vec)
{
	int rc = 0;
	struct smb2_write_req *req = NULL;
	struct smb2_write_rsp *rsp = NULL;
	int resp_buftype;
	*nbytes = 0;

	if (n_vec < 1)
		return rc;

	rc = small_smb2_init(SMB2_WRITE, io_parms->tcon, (void **) &req);
	if (rc)
		return rc;

	if (io_parms->tcon->ses->server == NULL)
		return -ECONNABORTED;

	req->hdr.ProcessId = cpu_to_le32(io_parms->pid);

	req->PersistentFileId = io_parms->persistent_fid;
	req->VolatileFileId = io_parms->volatile_fid;
	req->WriteChannelInfoOffset = 0;
	req->WriteChannelInfoLength = 0;
	req->Channel = 0;
	req->Length = cpu_to_le32(io_parms->length);
	req->Offset = cpu_to_le64(io_parms->offset);
	/* 4 for rfc1002 length field */
	req->DataOffset = cpu_to_le16(
				offsetof(struct smb2_write_req, Buffer) - 4);
	req->RemainingBytes = 0;

	iov[0].iov_base = (char *)req;
	/* 4 for rfc1002 length field and 1 for Buffer */
	iov[0].iov_len = get_rfc1002_length(req) + 4 - 1;

	/* length of entire message including data to be written */
	inc_rfc1001_len(req, io_parms->length - 1 /* Buffer */);

	rc = SendReceive2(xid, io_parms->tcon->ses, iov, n_vec + 1,
			  &resp_buftype, 0);
	rsp = (struct smb2_write_rsp *)iov[0].iov_base;

	if (rc) {
		cifs_stats_fail_inc(io_parms->tcon, SMB2_WRITE_HE);
		cifs_dbg(VFS, "Send error in write = %d\n", rc);
	} else
		*nbytes = le32_to_cpu(rsp->DataLength);

	free_rsp_buf(resp_buftype, rsp);
	return rc;
}
