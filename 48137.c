psf_ftruncate (SF_PRIVATE *psf, sf_count_t len)
{	int retval = 0 ;
	LONG lDistanceToMoveLow, lDistanceToMoveHigh ;
	DWORD dwResult, dwError = NO_ERROR ;

	/* This implementation trashes the current file position.
	** should it save and restore it? what if the current position is past
	** the new end of file?
	*/

	/* Returns 0 on success, non-zero on failure. */
	if (len < 0)
		return 1 ;

	lDistanceToMoveLow = (DWORD) (len & 0xFFFFFFFF) ;
	lDistanceToMoveHigh = (DWORD) ((len >> 32) & 0xFFFFFFFF) ;

	dwResult = SetFilePointer (psf->file.handle, lDistanceToMoveLow, &lDistanceToMoveHigh, FILE_BEGIN) ;

	if (dwResult == 0xFFFFFFFF)
		dwError = GetLastError () ;

	if (dwError != NO_ERROR)
	{	retval = -1 ;
		psf_log_syserr (psf, dwError) ;
		}
	else
	{	/* Note: when SetEndOfFile is used to extend a file, the contents of the
		** new portion of the file is undefined. This is unlike chsize(),
		** which guarantees that the new portion of the file will be zeroed.
		** Not sure if this is important or not.
		*/
		if (SetEndOfFile (psf->file.handle) == 0)
		{	retval = -1 ;
			psf_log_syserr (psf, GetLastError ()) ;
			} ;
		} ;

	return retval ;
} /* psf_ftruncate */
