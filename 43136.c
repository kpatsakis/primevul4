SMB2_query_info(const unsigned int xid, struct cifs_tcon *tcon,
		u64 persistent_fid, u64 volatile_fid,
		struct smb2_file_all_info *data)
{
	return query_info(xid, tcon, persistent_fid, volatile_fid,
			  FILE_ALL_INFORMATION,
			  sizeof(struct smb2_file_all_info) + MAX_NAME * 2,
			  sizeof(struct smb2_file_all_info), data);
}
