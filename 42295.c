static int udf_delete_entry(struct inode *inode, struct fileIdentDesc *fi,
			    struct udf_fileident_bh *fibh,
			    struct fileIdentDesc *cfi)
{
	cfi->fileCharacteristics |= FID_FILE_CHAR_DELETED;

	if (UDF_QUERY_FLAG(inode->i_sb, UDF_FLAG_STRICT))
		memset(&(cfi->icb), 0x00, sizeof(struct long_ad));

	return udf_write_fi(inode, cfi, fi, fibh, NULL, NULL);
}
