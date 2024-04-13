validate_buf(unsigned int offset, unsigned int buffer_length,
	     struct smb2_hdr *hdr, unsigned int min_buf_size)

{
	unsigned int smb_len = be32_to_cpu(hdr->smb2_buf_length);
	char *end_of_smb = smb_len + 4 /* RFC1001 length field */ + (char *)hdr;
	char *begin_of_buf = 4 /* RFC1001 len field */ + offset + (char *)hdr;
	char *end_of_buf = begin_of_buf + buffer_length;


	if (buffer_length < min_buf_size) {
		cifs_dbg(VFS, "buffer length %d smaller than minimum size %d\n",
			 buffer_length, min_buf_size);
		return -EINVAL;
	}

	/* check if beyond RFC1001 maximum length */
	if ((smb_len > 0x7FFFFF) || (buffer_length > 0x7FFFFF)) {
		cifs_dbg(VFS, "buffer length %d or smb length %d too large\n",
			 buffer_length, smb_len);
		return -EINVAL;
	}

	if ((begin_of_buf > end_of_smb) || (end_of_buf > end_of_smb)) {
		cifs_dbg(VFS, "illegal server response, bad offset to data\n");
		return -EINVAL;
	}

	return 0;
}
