validate_and_copy_buf(unsigned int offset, unsigned int buffer_length,
		      struct smb2_hdr *hdr, unsigned int minbufsize,
		      char *data)

{
	char *begin_of_buf = 4 /* RFC1001 len field */ + offset + (char *)hdr;
	int rc;

	if (!data)
		return -EINVAL;

	rc = validate_buf(offset, buffer_length, hdr, minbufsize);
	if (rc)
		return rc;

	memcpy(data, begin_of_buf, buffer_length);

	return 0;
}
