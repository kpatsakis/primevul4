struct cifs_ntsd *get_cifs_acl_by_fid(struct cifs_sb_info *cifs_sb,
		const struct cifs_fid *cifsfid, u32 *pacllen)
{
	struct cifs_ntsd *pntsd = NULL;
	unsigned int xid;
	int rc;
	struct tcon_link *tlink = cifs_sb_tlink(cifs_sb);

	if (IS_ERR(tlink))
		return ERR_CAST(tlink);

	xid = get_xid();
	rc = CIFSSMBGetCIFSACL(xid, tlink_tcon(tlink), cifsfid->netfid, &pntsd,
				pacllen);
	free_xid(xid);

	cifs_put_tlink(tlink);

	cifs_dbg(FYI, "%s: rc = %d ACL len %d\n", __func__, rc, *pacllen);
	if (rc)
		return ERR_PTR(rc);
	return pntsd;
}
