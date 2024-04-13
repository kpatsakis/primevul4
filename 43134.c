SMB2_open(const unsigned int xid, struct cifs_open_parms *oparms, __le16 *path,
	  __u8 *oplock, struct smb2_file_all_info *buf,
	  struct smb2_err_rsp **err_buf)
{
	struct smb2_create_req *req;
	struct smb2_create_rsp *rsp;
	struct TCP_Server_Info *server;
	struct cifs_tcon *tcon = oparms->tcon;
	struct cifs_ses *ses = tcon->ses;
	struct kvec iov[4];
	int resp_buftype;
	int uni_path_len;
	__le16 *copy_path = NULL;
	int copy_size;
	int rc = 0;
	unsigned int num_iovecs = 2;
	__u32 file_attributes = 0;
	char *dhc_buf = NULL, *lc_buf = NULL;

	cifs_dbg(FYI, "create/open\n");

	if (ses && (ses->server))
		server = ses->server;
	else
		return -EIO;

	rc = small_smb2_init(SMB2_CREATE, tcon, (void **) &req);
	if (rc)
		return rc;

	if (oparms->create_options & CREATE_OPTION_READONLY)
		file_attributes |= ATTR_READONLY;

	req->ImpersonationLevel = IL_IMPERSONATION;
	req->DesiredAccess = cpu_to_le32(oparms->desired_access);
	/* File attributes ignored on open (used in create though) */
	req->FileAttributes = cpu_to_le32(file_attributes);
	req->ShareAccess = FILE_SHARE_ALL_LE;
	req->CreateDisposition = cpu_to_le32(oparms->disposition);
	req->CreateOptions = cpu_to_le32(oparms->create_options & CREATE_OPTIONS_MASK);
	uni_path_len = (2 * UniStrnlen((wchar_t *)path, PATH_MAX)) + 2;
	/* do not count rfc1001 len field */
	req->NameOffset = cpu_to_le16(sizeof(struct smb2_create_req) - 4);

	iov[0].iov_base = (char *)req;
	/* 4 for rfc1002 length field */
	iov[0].iov_len = get_rfc1002_length(req) + 4;

	/* MUST set path len (NameLength) to 0 opening root of share */
	req->NameLength = cpu_to_le16(uni_path_len - 2);
	/* -1 since last byte is buf[0] which is sent below (path) */
	iov[0].iov_len--;
	if (uni_path_len % 8 != 0) {
		copy_size = uni_path_len / 8 * 8;
		if (copy_size < uni_path_len)
			copy_size += 8;

		copy_path = kzalloc(copy_size, GFP_KERNEL);
		if (!copy_path)
			return -ENOMEM;
		memcpy((char *)copy_path, (const char *)path,
			uni_path_len);
		uni_path_len = copy_size;
		path = copy_path;
	}

	iov[1].iov_len = uni_path_len;
	iov[1].iov_base = path;
	/* -1 since last byte is buf[0] which was counted in smb2_buf_len */
	inc_rfc1001_len(req, uni_path_len - 1);

	if (!server->oplocks)
		*oplock = SMB2_OPLOCK_LEVEL_NONE;

	if (!(server->capabilities & SMB2_GLOBAL_CAP_LEASING) ||
	    *oplock == SMB2_OPLOCK_LEVEL_NONE)
		req->RequestedOplockLevel = *oplock;
	else {
		rc = add_lease_context(server, iov, &num_iovecs, oplock);
		if (rc) {
			cifs_small_buf_release(req);
			kfree(copy_path);
			return rc;
		}
		lc_buf = iov[num_iovecs-1].iov_base;
	}

	if (*oplock == SMB2_OPLOCK_LEVEL_BATCH) {
		/* need to set Next field of lease context if we request it */
		if (server->capabilities & SMB2_GLOBAL_CAP_LEASING) {
			struct create_context *ccontext =
			    (struct create_context *)iov[num_iovecs-1].iov_base;
			ccontext->Next =
				cpu_to_le32(server->vals->create_lease_size);
		}
		rc = add_durable_context(iov, &num_iovecs, oparms);
		if (rc) {
			cifs_small_buf_release(req);
			kfree(copy_path);
			kfree(lc_buf);
			return rc;
		}
		dhc_buf = iov[num_iovecs-1].iov_base;
	}

	rc = SendReceive2(xid, ses, iov, num_iovecs, &resp_buftype, 0);
	rsp = (struct smb2_create_rsp *)iov[0].iov_base;

	if (rc != 0) {
		cifs_stats_fail_inc(tcon, SMB2_CREATE_HE);
		if (err_buf)
			*err_buf = kmemdup(rsp, get_rfc1002_length(rsp) + 4,
					   GFP_KERNEL);
		goto creat_exit;
	}

	oparms->fid->persistent_fid = rsp->PersistentFileId;
	oparms->fid->volatile_fid = rsp->VolatileFileId;

	if (buf) {
		memcpy(buf, &rsp->CreationTime, 32);
		buf->AllocationSize = rsp->AllocationSize;
		buf->EndOfFile = rsp->EndofFile;
		buf->Attributes = rsp->FileAttributes;
		buf->NumberOfLinks = cpu_to_le32(1);
		buf->DeletePending = 0;
	}

	if (rsp->OplockLevel == SMB2_OPLOCK_LEVEL_LEASE)
		*oplock = parse_lease_state(server, rsp, &oparms->fid->epoch);
	else
		*oplock = rsp->OplockLevel;
creat_exit:
	kfree(copy_path);
	kfree(lc_buf);
	kfree(dhc_buf);
	free_rsp_buf(resp_buftype, rsp);
	return rc;
}
