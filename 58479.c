flac_init (SF_PRIVATE *psf)
{
	if (psf->file.mode == SFM_RDWR)
		return SFE_BAD_MODE_RW ;

	if (psf->file.mode == SFM_READ)
	{	psf->read_short		= flac_read_flac2s ;
		psf->read_int		= flac_read_flac2i ;
		psf->read_float		= flac_read_flac2f ;
		psf->read_double	= flac_read_flac2d ;
		} ;

	if (psf->file.mode == SFM_WRITE)
	{	psf->write_short	= flac_write_s2flac ;
		psf->write_int		= flac_write_i2flac ;
		psf->write_float	= flac_write_f2flac ;
		psf->write_double	= flac_write_d2flac ;
		} ;

	if (psf->filelength > psf->dataoffset)
		psf->datalength = (psf->dataend) ? psf->dataend - psf->dataoffset : psf->filelength - psf->dataoffset ;
	else
		psf->datalength = 0 ;

	return 0 ;
} /* flac_init */
