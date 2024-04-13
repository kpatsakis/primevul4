flac_byterate (SF_PRIVATE *psf)
{
	if (psf->file.mode == SFM_READ)
		return (psf->datalength * psf->sf.samplerate) / psf->sf.frames ;

	return -1 ;
} /* flac_byterate */
