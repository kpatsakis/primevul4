smb2_new_read_req(struct kvec *iov, struct cifs_io_parms *io_parms,
		  unsigned int remaining_bytes, int request_type)
{
	int rc = -EACCES;
	struct smb2_read_req *req = NULL;

	rc = small_smb2_init(SMB2_READ, io_parms->tcon, (void **) &req);
	if (rc)
		return rc;
	if (io_parms->tcon->ses->server == NULL)
		return -ECONNABORTED;

	req->hdr.ProcessId = cpu_to_le32(io_parms->pid);

	req->PersistentFileId = io_parms->persistent_fid;
	req->VolatileFileId = io_parms->volatile_fid;
	req->ReadChannelInfoOffset = 0; /* reserved */
	req->ReadChannelInfoLength = 0; /* reserved */
	req->Channel = 0; /* reserved */
	req->MinimumCount = 0;
	req->Length = cpu_to_le32(io_parms->length);
	req->Offset = cpu_to_le64(io_parms->offset);

	if (request_type & CHAINED_REQUEST) {
		if (!(request_type & END_OF_CHAIN)) {
			/* 4 for rfc1002 length field */
			req->hdr.NextCommand =
				cpu_to_le32(get_rfc1002_length(req) + 4);
		} else /* END_OF_CHAIN */
			req->hdr.NextCommand = 0;
		if (request_type & RELATED_REQUEST) {
			req->hdr.Flags |= SMB2_FLAGS_RELATED_OPERATIONS;
			/*
			 * Related requests use info from previous read request
			 * in chain.
			 */
			req->hdr.SessionId = 0xFFFFFFFF;
			req->hdr.TreeId = 0xFFFFFFFF;
			req->PersistentFileId = 0xFFFFFFFF;
			req->VolatileFileId = 0xFFFFFFFF;
		}
	}
	if (remaining_bytes > io_parms->length)
		req->RemainingBytes = cpu_to_le32(remaining_bytes);
	else
		req->RemainingBytes = 0;

	iov[0].iov_base = (char *)req;
	/* 4 for rfc1002 length field */
	iov[0].iov_len = get_rfc1002_length(req) + 4;
	return rc;
}
