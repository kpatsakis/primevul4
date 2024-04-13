int fmtutil_find_zip_eocd(deark *c, dbuf *f, i64 *foundpos)
{
	u32 sig;
	u8 *buf = NULL;
	int retval = 0;
	i64 buf_offset;
	i64 buf_size;
	i64 i;

	*foundpos = 0;
	if(f->len < 22) goto done;

	// End-of-central-dir record usually starts 22 bytes from EOF. Try that first.
	sig = (u32)dbuf_getu32le(f, f->len - 22);
	if(sig == 0x06054b50U) {
		*foundpos = f->len - 22;
		retval = 1;
		goto done;
	}

	// Search for the signature.
	// The end-of-central-directory record could theoretically appear anywhere
	// in the file. We'll follow Info-Zip/UnZip's lead and search the last 66000
	// bytes.
#define MAX_ZIP_EOCD_SEARCH 66000
	buf_size = f->len;
	if(buf_size > MAX_ZIP_EOCD_SEARCH) buf_size = MAX_ZIP_EOCD_SEARCH;

	buf = de_malloc(c, buf_size);
	buf_offset = f->len - buf_size;
	dbuf_read(f, buf, buf_offset, buf_size);

	for(i=buf_size-22; i>=0; i--) {
		if(buf[i]=='P' && buf[i+1]=='K' && buf[i+2]==5 && buf[i+3]==6) {
			*foundpos = buf_offset + i;
			retval = 1;
			goto done;
		}
	}

done:
	de_free(c, buf);
	return retval;
}