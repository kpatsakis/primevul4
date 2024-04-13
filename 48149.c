psf_log_syserr (SF_PRIVATE *psf, int error)
{	LPVOID lpMsgBuf ;

	/* Only log an error if no error has been set yet. */
	if (psf->error == 0)
	{	psf->error = SFE_SYSTEM ;

		FormatMessage (
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			error,
			MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0,
			NULL
			) ;

		snprintf (psf->syserr, sizeof (psf->syserr), "System error : %s", (char*) lpMsgBuf) ;
		LocalFree (lpMsgBuf) ;
		} ;

	return ;
} /* psf_log_syserr */
