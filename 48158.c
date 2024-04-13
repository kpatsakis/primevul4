psf_set_stdio (SF_PRIVATE *psf)
{	HANDLE	handle = NULL ;
	int	error = 0 ;

	switch (psf->file.mode)
	{	case SFM_RDWR :
				error = SFE_OPEN_PIPE_RDWR ;
				break ;

		case SFM_READ :
				handle = GetStdHandle (STD_INPUT_HANDLE) ;
				psf->file.do_not_close_descriptor = 1 ;
				break ;

		case SFM_WRITE :
				handle = GetStdHandle (STD_OUTPUT_HANDLE) ;
				psf->file.do_not_close_descriptor = 1 ;
				break ;

		default :
				error = SFE_BAD_OPEN_MODE ;
				break ;
		} ;

	psf->file.handle = handle ;
	psf->filelength = 0 ;

	return error ;
} /* psf_set_stdio */
