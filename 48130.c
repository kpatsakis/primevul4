psf_fseek (SF_PRIVATE *psf, sf_count_t offset, int whence)
{	sf_count_t new_position ;
	LONG lDistanceToMove, lDistanceToMoveHigh ;
	DWORD dwMoveMethod ;
	DWORD dwResult, dwError ;

	if (psf->virtual_io)
		return psf->vio.seek (offset, whence, psf->vio_user_data) ;

	switch (whence)
	{	case SEEK_SET :
				offset += psf->fileoffset ;
				dwMoveMethod = FILE_BEGIN ;
				break ;

		case SEEK_END :
				dwMoveMethod = FILE_END ;
				break ;

		default :
				dwMoveMethod = FILE_CURRENT ;
				break ;
		} ;

	lDistanceToMove = (DWORD) (offset & 0xFFFFFFFF) ;
	lDistanceToMoveHigh = (DWORD) ((offset >> 32) & 0xFFFFFFFF) ;

	dwResult = SetFilePointer (psf->file.handle, lDistanceToMove, &lDistanceToMoveHigh, dwMoveMethod) ;

	if (dwResult == 0xFFFFFFFF)
		dwError = GetLastError () ;
	else
		dwError = NO_ERROR ;

	if (dwError != NO_ERROR)
	{	psf_log_syserr (psf, dwError) ;
		return -1 ;
		} ;

	new_position = (dwResult + ((__int64) lDistanceToMoveHigh << 32)) - psf->fileoffset ;

	return new_position ;
} /* psf_fseek */
