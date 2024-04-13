SMB2_QFS_info(const unsigned int xid, struct cifs_tcon *tcon,
	      u64 persistent_fid, u64 volatile_fid, struct kstatfs *fsdata)
{
	struct smb2_query_info_rsp *rsp = NULL;
	struct kvec iov;
	int rc = 0;
	int resp_buftype;
	struct cifs_ses *ses = tcon->ses;
	struct smb2_fs_full_size_info *info = NULL;

	rc = build_qfs_info_req(&iov, tcon, FS_FULL_SIZE_INFORMATION,
				sizeof(struct smb2_fs_full_size_info),
				persistent_fid, volatile_fid);
	if (rc)
		return rc;

	rc = SendReceive2(xid, ses, &iov, 1, &resp_buftype, 0);
	if (rc) {
		cifs_stats_fail_inc(tcon, SMB2_QUERY_INFO_HE);
		goto qfsinf_exit;
	}
	rsp = (struct smb2_query_info_rsp *)iov.iov_base;

	info = (struct smb2_fs_full_size_info *)(4 /* RFC1001 len */ +
		le16_to_cpu(rsp->OutputBufferOffset) + (char *)&rsp->hdr);
	rc = validate_buf(le16_to_cpu(rsp->OutputBufferOffset),
			  le32_to_cpu(rsp->OutputBufferLength), &rsp->hdr,
			  sizeof(struct smb2_fs_full_size_info));
	if (!rc)
		copy_fs_info_to_kstatfs(info, fsdata);

qfsinf_exit:
	free_rsp_buf(resp_buftype, iov.iov_base);
	return rc;
}
