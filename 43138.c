SMB2_rename(const unsigned int xid, struct cifs_tcon *tcon,
	    u64 persistent_fid, u64 volatile_fid, __le16 *target_file)
{
	struct smb2_file_rename_info info;
	void **data;
	unsigned int size[2];
	int rc;
	int len = (2 * UniStrnlen((wchar_t *)target_file, PATH_MAX));

	data = kmalloc(sizeof(void *) * 2, GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	info.ReplaceIfExists = 1; /* 1 = replace existing target with new */
			      /* 0 = fail if target already exists */
	info.RootDirectory = 0;  /* MBZ for network ops (why does spec say?) */
	info.FileNameLength = cpu_to_le32(len);

	data[0] = &info;
	size[0] = sizeof(struct smb2_file_rename_info);

	data[1] = target_file;
	size[1] = len + 2 /* null */;

	rc = send_set_info(xid, tcon, persistent_fid, volatile_fid,
			   current->tgid, FILE_RENAME_INFORMATION, 2, data,
			   size);
	kfree(data);
	return rc;
}
