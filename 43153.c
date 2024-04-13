send_set_info(const unsigned int xid, struct cifs_tcon *tcon,
	       u64 persistent_fid, u64 volatile_fid, u32 pid, int info_class,
	       unsigned int num, void **data, unsigned int *size)
{
	struct smb2_set_info_req *req;
	struct smb2_set_info_rsp *rsp = NULL;
	struct kvec *iov;
	int rc = 0;
	int resp_buftype;
	unsigned int i;
	struct TCP_Server_Info *server;
	struct cifs_ses *ses = tcon->ses;

	if (ses && (ses->server))
		server = ses->server;
	else
		return -EIO;

	if (!num)
		return -EINVAL;

	iov = kmalloc(sizeof(struct kvec) * num, GFP_KERNEL);
	if (!iov)
		return -ENOMEM;

	rc = small_smb2_init(SMB2_SET_INFO, tcon, (void **) &req);
	if (rc) {
		kfree(iov);
		return rc;
	}

	req->hdr.ProcessId = cpu_to_le32(pid);

	req->InfoType = SMB2_O_INFO_FILE;
	req->FileInfoClass = info_class;
	req->PersistentFileId = persistent_fid;
	req->VolatileFileId = volatile_fid;

	/* 4 for RFC1001 length and 1 for Buffer */
	req->BufferOffset =
			cpu_to_le16(sizeof(struct smb2_set_info_req) - 1 - 4);
	req->BufferLength = cpu_to_le32(*size);

	inc_rfc1001_len(req, *size - 1 /* Buffer */);

	memcpy(req->Buffer, *data, *size);

	iov[0].iov_base = (char *)req;
	/* 4 for RFC1001 length */
	iov[0].iov_len = get_rfc1002_length(req) + 4;

	for (i = 1; i < num; i++) {
		inc_rfc1001_len(req, size[i]);
		le32_add_cpu(&req->BufferLength, size[i]);
		iov[i].iov_base = (char *)data[i];
		iov[i].iov_len = size[i];
	}

	rc = SendReceive2(xid, ses, iov, num, &resp_buftype, 0);
	rsp = (struct smb2_set_info_rsp *)iov[0].iov_base;

	if (rc != 0)
		cifs_stats_fail_inc(tcon, SMB2_SET_INFO_HE);

	free_rsp_buf(resp_buftype, rsp);
	kfree(iov);
	return rc;
}
