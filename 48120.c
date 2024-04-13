psf_fgets (char *buffer, sf_count_t bufsize, SF_PRIVATE *psf)
{	sf_count_t k = 0 ;
	sf_count_t count ;
	DWORD dwNumberOfBytesRead ;

	while (k < bufsize - 1)
	{	if (ReadFile (psf->file.handle, &(buffer [k]), 1, &dwNumberOfBytesRead, 0) == 0)
		{	psf_log_syserr (psf, GetLastError ()) ;
			break ;
			}
		else
		{	count = dwNumberOfBytesRead ;
			/* note that we only check for '\n' not other line endings such as CRLF */
			if (count == 0 || buffer [k++] == '\n')
				break ;
			} ;
		} ;

	buffer [k] = 0 ;

	return k ;
} /* psf_fgets */
