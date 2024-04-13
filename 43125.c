SMB2_QFS_attr(const unsigned int xid, struct cifs_tcon *tcon,
	      u64 persistent_fid, u64 volatile_fid, int level)
{
	struct smb2_query_info_rsp *rsp = NULL;
	struct kvec iov;
	int rc = 0;
	int resp_buftype, max_len, min_len;
	struct cifs_ses *ses = tcon->ses;
	unsigned int rsp_len, offset;

	if (level == FS_DEVICE_INFORMATION) {
		max_len = sizeof(FILE_SYSTEM_DEVICE_INFO);
		min_len = sizeof(FILE_SYSTEM_DEVICE_INFO);
	} else if (level == FS_ATTRIBUTE_INFORMATION) {
		max_len = sizeof(FILE_SYSTEM_ATTRIBUTE_INFO);
		min_len = MIN_FS_ATTR_INFO_SIZE;
	} else if (level == FS_SECTOR_SIZE_INFORMATION) {
		max_len = sizeof(struct smb3_fs_ss_info);
		min_len = sizeof(struct smb3_fs_ss_info);
	} else {
		cifs_dbg(FYI, "Invalid qfsinfo level %d\n", level);
		return -EINVAL;
	}

	rc = build_qfs_info_req(&iov, tcon, level, max_len,
				persistent_fid, volatile_fid);
	if (rc)
		return rc;

	rc = SendReceive2(xid, ses, &iov, 1, &resp_buftype, 0);
	if (rc) {
		cifs_stats_fail_inc(tcon, SMB2_QUERY_INFO_HE);
		goto qfsattr_exit;
	}
	rsp = (struct smb2_query_info_rsp *)iov.iov_base;

	rsp_len = le32_to_cpu(rsp->OutputBufferLength);
	offset = le16_to_cpu(rsp->OutputBufferOffset);
	rc = validate_buf(offset, rsp_len, &rsp->hdr, min_len);
	if (rc)
		goto qfsattr_exit;

	if (level == FS_ATTRIBUTE_INFORMATION)
		memcpy(&tcon->fsAttrInfo, 4 /* RFC1001 len */ + offset
			+ (char *)&rsp->hdr, min_t(unsigned int,
			rsp_len, max_len));
	else if (level == FS_DEVICE_INFORMATION)
		memcpy(&tcon->fsDevInfo, 4 /* RFC1001 len */ + offset
			+ (char *)&rsp->hdr, sizeof(FILE_SYSTEM_DEVICE_INFO));
	else if (level == FS_SECTOR_SIZE_INFORMATION) {
		struct smb3_fs_ss_info *ss_info = (struct smb3_fs_ss_info *)
			(4 /* RFC1001 len */ + offset + (char *)&rsp->hdr);
		tcon->ss_flags = le32_to_cpu(ss_info->Flags);
		tcon->perf_sector_size =
			le32_to_cpu(ss_info->PhysicalBytesPerSectorForPerf);
	}

qfsattr_exit:
	free_rsp_buf(resp_buftype, iov.iov_base);
	return rc;
}
