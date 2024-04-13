_gd2ReadChunk (int offset, char *compBuf, int compSize, char *chunkBuf,
               uLongf * chunkLen, gdIOCtx * in)
{
	int zerr;

	if (gdTell (in) != offset) {
		GD2_DBG (printf ("Positioning in file to %d\n", offset));
		gdSeek (in, offset);
	} else {
		GD2_DBG (printf ("Already Positioned in file to %d\n", offset));
	};

	/* Read and uncompress an entire chunk. */
	GD2_DBG (printf ("Reading file\n"));
	if (gdGetBuf (compBuf, compSize, in) != compSize) {
		return FALSE;
	};
	GD2_DBG (printf
	         ("Got %d bytes. Uncompressing into buffer of %d bytes\n", compSize,
	          *chunkLen));
	zerr =
	    uncompress ((unsigned char *) chunkBuf, chunkLen,
	                (unsigned char *) compBuf, compSize);
	if (zerr != Z_OK) {
		GD2_DBG (printf ("Error %d from uncompress\n", zerr));
		return FALSE;
	};
	GD2_DBG (printf ("Got chunk\n"));
	return TRUE;
}
