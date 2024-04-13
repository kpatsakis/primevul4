flac_open	(SF_PRIVATE *psf)
{
	psf_log_printf (psf, "This version of libsndfile was compiled without FLAC support.\n") ;
	return SFE_UNIMPLEMENTED ;
} /* flac_open */
