psf_ftell (SF_PRIVATE *psf)
{	sf_count_t pos ;
	LONG lDistanceToMoveLow, lDistanceToMoveHigh ;
	DWORD dwResult, dwError ;

	if (psf->virtual_io)
		return psf->vio.tell (psf->vio_user_data) ;

	if (psf->is_pipe)
		return psf->pipeoffset ;

	lDistanceToMoveLow = 0 ;
	lDistanceToMoveHigh = 0 ;

	dwResult = SetFilePointer (psf->file.handle, lDistanceToMoveLow, &lDistanceToMoveHigh, FILE_CURRENT) ;

	if (dwResult == 0xFFFFFFFF)
		dwError = GetLastError () ;
	else
		dwError = NO_ERROR ;

	if (dwError != NO_ERROR)
	{	psf_log_syserr (psf, dwError) ;
		return -1 ;
		} ;

	pos = (dwResult + ((__int64) lDistanceToMoveHigh << 32)) ;

	return pos - psf->fileoffset ;
} /* psf_ftell */
