psf_set_stdio (SF_PRIVATE *psf)
{	int	error = 0 ;

	switch (psf->file.mode)
	{	case SFM_RDWR :
				error = SFE_OPEN_PIPE_RDWR ;
				break ;

		case SFM_READ :
				psf->file.filedes = 0 ;
				break ;

		case SFM_WRITE :
				psf->file.filedes = 1 ;
				break ;

		default :
				error = SFE_BAD_OPEN_MODE ;
				break ;
		} ;
	psf->filelength = 0 ;

	return error ;
} /* psf_set_stdio */
