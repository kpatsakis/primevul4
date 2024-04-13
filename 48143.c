psf_get_filelen_handle (HANDLE handle)
{	sf_count_t filelen ;
	DWORD dwFileSizeLow, dwFileSizeHigh, dwError = NO_ERROR ;

	dwFileSizeLow = GetFileSize (handle, &dwFileSizeHigh) ;

	if (dwFileSizeLow == 0xFFFFFFFF)
		dwError = GetLastError () ;

	if (dwError != NO_ERROR)
		return (sf_count_t) -1 ;

	filelen = dwFileSizeLow + ((__int64) dwFileSizeHigh << 32) ;

	return filelen ;
} /* psf_get_filelen_handle */
