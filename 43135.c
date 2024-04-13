SMB2_oplock_break(const unsigned int xid, struct cifs_tcon *tcon,
		  const u64 persistent_fid, const u64 volatile_fid,
		  __u8 oplock_level)
{
	int rc;
	struct smb2_oplock_break *req = NULL;

	cifs_dbg(FYI, "SMB2_oplock_break\n");
	rc = small_smb2_init(SMB2_OPLOCK_BREAK, tcon, (void **) &req);

	if (rc)
		return rc;

	req->VolatileFid = volatile_fid;
	req->PersistentFid = persistent_fid;
	req->OplockLevel = oplock_level;
	req->hdr.CreditRequest = cpu_to_le16(1);

	rc = SendReceiveNoRsp(xid, tcon->ses, (char *) req, CIFS_OBREAK_OP);
	/* SMB2 buffer freed by function above */

	if (rc) {
		cifs_stats_fail_inc(tcon, SMB2_OPLOCK_BREAK_HE);
		cifs_dbg(FYI, "Send error in Oplock Break = %d\n", rc);
	}

	return rc;
}
