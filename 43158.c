smb2_lockv(const unsigned int xid, struct cifs_tcon *tcon,
	   const __u64 persist_fid, const __u64 volatile_fid, const __u32 pid,
	   const __u32 num_lock, struct smb2_lock_element *buf)
{
	int rc = 0;
	struct smb2_lock_req *req = NULL;
	struct kvec iov[2];
	int resp_buf_type;
	unsigned int count;

	cifs_dbg(FYI, "smb2_lockv num lock %d\n", num_lock);

	rc = small_smb2_init(SMB2_LOCK, tcon, (void **) &req);
	if (rc)
		return rc;

	req->hdr.ProcessId = cpu_to_le32(pid);
	req->LockCount = cpu_to_le16(num_lock);

	req->PersistentFileId = persist_fid;
	req->VolatileFileId = volatile_fid;

	count = num_lock * sizeof(struct smb2_lock_element);
	inc_rfc1001_len(req, count - sizeof(struct smb2_lock_element));

	iov[0].iov_base = (char *)req;
	/* 4 for rfc1002 length field and count for all locks */
	iov[0].iov_len = get_rfc1002_length(req) + 4 - count;
	iov[1].iov_base = (char *)buf;
	iov[1].iov_len = count;

	cifs_stats_inc(&tcon->stats.cifs_stats.num_locks);
	rc = SendReceive2(xid, tcon->ses, iov, 2, &resp_buf_type, CIFS_NO_RESP);
	if (rc) {
		cifs_dbg(FYI, "Send error in smb2_lockv = %d\n", rc);
		cifs_stats_fail_inc(tcon, SMB2_LOCK_HE);
	}

	return rc;
}
