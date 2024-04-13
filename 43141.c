SMB2_set_info(const unsigned int xid, struct cifs_tcon *tcon,
	      u64 persistent_fid, u64 volatile_fid, FILE_BASIC_INFO *buf)
{
	unsigned int size;
	size = sizeof(FILE_BASIC_INFO);
	return send_set_info(xid, tcon, persistent_fid, volatile_fid,
			     current->tgid, FILE_BASIC_INFORMATION, 1,
			     (void **)&buf, &size);
}
