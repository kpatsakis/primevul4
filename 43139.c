SMB2_sess_setup(const unsigned int xid, struct cifs_ses *ses,
		const struct nls_table *nls_cp)
{
	struct smb2_sess_setup_req *req;
	struct smb2_sess_setup_rsp *rsp = NULL;
	struct kvec iov[2];
	int rc = 0;
	int resp_buftype;
	__le32 phase = NtLmNegotiate; /* NTLMSSP, if needed, is multistage */
	struct TCP_Server_Info *server = ses->server;
	u16 blob_length = 0;
	char *security_blob;
	char *ntlmssp_blob = NULL;
	bool use_spnego = false; /* else use raw ntlmssp */

	cifs_dbg(FYI, "Session Setup\n");

	if (!server) {
		WARN(1, "%s: server is NULL!\n", __func__);
		return -EIO;
	}

	/*
	 * If we are here due to reconnect, free per-smb session key
	 * in case signing was required.
	 */
	kfree(ses->auth_key.response);
	ses->auth_key.response = NULL;

	/*
	 * If memory allocation is successful, caller of this function
	 * frees it.
	 */
	ses->ntlmssp = kmalloc(sizeof(struct ntlmssp_auth), GFP_KERNEL);
	if (!ses->ntlmssp)
		return -ENOMEM;
	ses->ntlmssp->sesskey_per_smbsess = true;

	/* FIXME: allow for other auth types besides NTLMSSP (e.g. krb5) */
	ses->sectype = RawNTLMSSP;

ssetup_ntlmssp_authenticate:
	if (phase == NtLmChallenge)
		phase = NtLmAuthenticate; /* if ntlmssp, now final phase */

	rc = small_smb2_init(SMB2_SESSION_SETUP, NULL, (void **) &req);
	if (rc)
		return rc;

	req->hdr.SessionId = 0; /* First session, not a reauthenticate */
	req->VcNumber = 0; /* MBZ */
	/* to enable echos and oplocks */
	req->hdr.CreditRequest = cpu_to_le16(3);

	/* only one of SMB2 signing flags may be set in SMB2 request */
	if (server->sign)
		req->SecurityMode = SMB2_NEGOTIATE_SIGNING_REQUIRED;
	else if (global_secflags & CIFSSEC_MAY_SIGN) /* one flag unlike MUST_ */
		req->SecurityMode = SMB2_NEGOTIATE_SIGNING_ENABLED;
	else
		req->SecurityMode = 0;

	req->Capabilities = 0;
	req->Channel = 0; /* MBZ */

	iov[0].iov_base = (char *)req;
	/* 4 for rfc1002 length field and 1 for pad */
	iov[0].iov_len = get_rfc1002_length(req) + 4 - 1;
	if (phase == NtLmNegotiate) {
		ntlmssp_blob = kmalloc(sizeof(struct _NEGOTIATE_MESSAGE),
				       GFP_KERNEL);
		if (ntlmssp_blob == NULL) {
			rc = -ENOMEM;
			goto ssetup_exit;
		}
		build_ntlmssp_negotiate_blob(ntlmssp_blob, ses);
		if (use_spnego) {
			/* blob_length = build_spnego_ntlmssp_blob(
					&security_blob,
					sizeof(struct _NEGOTIATE_MESSAGE),
					ntlmssp_blob); */
			/* BB eventually need to add this */
			cifs_dbg(VFS, "spnego not supported for SMB2 yet\n");
			rc = -EOPNOTSUPP;
			kfree(ntlmssp_blob);
			goto ssetup_exit;
		} else {
			blob_length = sizeof(struct _NEGOTIATE_MESSAGE);
			/* with raw NTLMSSP we don't encapsulate in SPNEGO */
			security_blob = ntlmssp_blob;
		}
	} else if (phase == NtLmAuthenticate) {
		req->hdr.SessionId = ses->Suid;
		ntlmssp_blob = kzalloc(sizeof(struct _NEGOTIATE_MESSAGE) + 500,
				       GFP_KERNEL);
		if (ntlmssp_blob == NULL) {
			rc = -ENOMEM;
			goto ssetup_exit;
		}
		rc = build_ntlmssp_auth_blob(ntlmssp_blob, &blob_length, ses,
					     nls_cp);
		if (rc) {
			cifs_dbg(FYI, "build_ntlmssp_auth_blob failed %d\n",
				 rc);
			goto ssetup_exit; /* BB double check error handling */
		}
		if (use_spnego) {
			/* blob_length = build_spnego_ntlmssp_blob(
							&security_blob,
							blob_length,
							ntlmssp_blob); */
			cifs_dbg(VFS, "spnego not supported for SMB2 yet\n");
			rc = -EOPNOTSUPP;
			kfree(ntlmssp_blob);
			goto ssetup_exit;
		} else {
			security_blob = ntlmssp_blob;
		}
	} else {
		cifs_dbg(VFS, "illegal ntlmssp phase\n");
		rc = -EIO;
		goto ssetup_exit;
	}

	/* Testing shows that buffer offset must be at location of Buffer[0] */
	req->SecurityBufferOffset =
				cpu_to_le16(sizeof(struct smb2_sess_setup_req) -
					    1 /* pad */ - 4 /* rfc1001 len */);
	req->SecurityBufferLength = cpu_to_le16(blob_length);
	iov[1].iov_base = security_blob;
	iov[1].iov_len = blob_length;

	inc_rfc1001_len(req, blob_length - 1 /* pad */);

	/* BB add code to build os and lm fields */

	rc = SendReceive2(xid, ses, iov, 2, &resp_buftype,
			  CIFS_LOG_ERROR | CIFS_NEG_OP);

	kfree(security_blob);
	rsp = (struct smb2_sess_setup_rsp *)iov[0].iov_base;
	if (resp_buftype != CIFS_NO_BUFFER &&
	    rsp->hdr.Status == STATUS_MORE_PROCESSING_REQUIRED) {
		if (phase != NtLmNegotiate) {
			cifs_dbg(VFS, "Unexpected more processing error\n");
			goto ssetup_exit;
		}
		if (offsetof(struct smb2_sess_setup_rsp, Buffer) - 4 !=
				le16_to_cpu(rsp->SecurityBufferOffset)) {
			cifs_dbg(VFS, "Invalid security buffer offset %d\n",
				 le16_to_cpu(rsp->SecurityBufferOffset));
			rc = -EIO;
			goto ssetup_exit;
		}

		/* NTLMSSP Negotiate sent now processing challenge (response) */
		phase = NtLmChallenge; /* process ntlmssp challenge */
		rc = 0; /* MORE_PROCESSING is not an error here but expected */
		ses->Suid = rsp->hdr.SessionId;
		rc = decode_ntlmssp_challenge(rsp->Buffer,
				le16_to_cpu(rsp->SecurityBufferLength), ses);
	}

	/*
	 * BB eventually add code for SPNEGO decoding of NtlmChallenge blob,
	 * but at least the raw NTLMSSP case works.
	 */
	/*
	 * No tcon so can't do
	 * cifs_stats_inc(&tcon->stats.smb2_stats.smb2_com_fail[SMB2...]);
	 */
	if (rc != 0)
		goto ssetup_exit;

	ses->session_flags = le16_to_cpu(rsp->SessionFlags);
	if (ses->session_flags & SMB2_SESSION_FLAG_ENCRYPT_DATA)
		cifs_dbg(VFS, "SMB3 encryption not supported yet\n");
ssetup_exit:
	free_rsp_buf(resp_buftype, rsp);

	/* if ntlmssp, and negotiate succeeded, proceed to authenticate phase */
	if ((phase == NtLmChallenge) && (rc == 0))
		goto ssetup_ntlmssp_authenticate;

	if (!rc) {
		mutex_lock(&server->srv_mutex);
		if (server->sign && server->ops->generate_signingkey) {
			rc = server->ops->generate_signingkey(ses);
			kfree(ses->auth_key.response);
			ses->auth_key.response = NULL;
			if (rc) {
				cifs_dbg(FYI,
					"SMB3 session key generation failed\n");
				mutex_unlock(&server->srv_mutex);
				goto keygen_exit;
			}
		}
		if (!server->session_estab) {
			server->sequence_number = 0x2;
			server->session_estab = true;
		}
		mutex_unlock(&server->srv_mutex);

		cifs_dbg(FYI, "SMB2/3 session established successfully\n");
		spin_lock(&GlobalMid_Lock);
		ses->status = CifsGood;
		ses->need_reconnect = false;
		spin_unlock(&GlobalMid_Lock);
	}

keygen_exit:
	if (!server->sign) {
		kfree(ses->auth_key.response);
		ses->auth_key.response = NULL;
	}
	kfree(ses->ntlmssp);

	return rc;
}
