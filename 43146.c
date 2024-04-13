build_qfs_info_req(struct kvec *iov, struct cifs_tcon *tcon, int level,
		   int outbuf_len, u64 persistent_fid, u64 volatile_fid)
{
	int rc;
	struct smb2_query_info_req *req;

	cifs_dbg(FYI, "Query FSInfo level %d\n", level);

	if ((tcon->ses == NULL) || (tcon->ses->server == NULL))
		return -EIO;

	rc = small_smb2_init(SMB2_QUERY_INFO, tcon, (void **) &req);
	if (rc)
		return rc;

	req->InfoType = SMB2_O_INFO_FILESYSTEM;
	req->FileInfoClass = level;
	req->PersistentFileId = persistent_fid;
	req->VolatileFileId = volatile_fid;
	/* 4 for rfc1002 length field and 1 for pad */
	req->InputBufferOffset =
			cpu_to_le16(sizeof(struct smb2_query_info_req) - 1 - 4);
	req->OutputBufferLength = cpu_to_le32(
		outbuf_len + sizeof(struct smb2_query_info_rsp) - 1 - 4);

	iov->iov_base = (char *)req;
	/* 4 for rfc1002 length field */
	iov->iov_len = get_rfc1002_length(req) + 4;
	return 0;
}
