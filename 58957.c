aiff_read_basc_chunk (SF_PRIVATE * psf, int datasize)
{	const char * type_str ;
	basc_CHUNK bc ;
	int count ;

	count = psf_binheader_readf (psf, "E442", &bc.version, &bc.numBeats, &bc.rootNote) ;
	count += psf_binheader_readf (psf, "E222", &bc.scaleType, &bc.sigNumerator, &bc.sigDenominator) ;
	count += psf_binheader_readf (psf, "E2j", &bc.loopType, datasize - sizeof (bc)) ;

	psf_log_printf (psf, "  Version ? : %u\n  Num Beats : %u\n  Root Note : 0x%x\n",
						bc.version, bc.numBeats, bc.rootNote) ;

	switch (bc.scaleType)
	{	case basc_SCALE_MINOR :
				type_str = "MINOR" ;
				break ;
		case basc_SCALE_MAJOR :
				type_str = "MAJOR" ;
				break ;
		case basc_SCALE_NEITHER :
				type_str = "NEITHER" ;
				break ;
		case basc_SCALE_BOTH :
				type_str = "BOTH" ;
				break ;
		default :
				type_str = "!!WRONG!!" ;
				break ;
		} ;

	psf_log_printf (psf, "  ScaleType : 0x%x (%s)\n", bc.scaleType, type_str) ;
	psf_log_printf (psf, "  Time Sig  : %d/%d\n", bc.sigNumerator, bc.sigDenominator) ;

	switch (bc.loopType)
	{	case basc_TYPE_ONE_SHOT :
				type_str = "One Shot" ;
				break ;
		case basc_TYPE_LOOP :
				type_str = "Loop" ;
				break ;
		default:
				type_str = "!!WRONG!!" ;
				break ;
		} ;

	psf_log_printf (psf, "  Loop Type : 0x%x (%s)\n", bc.loopType, type_str) ;

	if ((psf->loop_info = calloc (1, sizeof (SF_LOOP_INFO))) == NULL)
		return SFE_MALLOC_FAILED ;

	psf->loop_info->time_sig_num	= bc.sigNumerator ;
	psf->loop_info->time_sig_den	= bc.sigDenominator ;
	psf->loop_info->loop_mode		= (bc.loopType == basc_TYPE_ONE_SHOT) ? SF_LOOP_NONE : SF_LOOP_FORWARD ;
	psf->loop_info->num_beats		= bc.numBeats ;

	/* Can always be recalculated from other known fields. */
	psf->loop_info->bpm = (1.0 / psf->sf.frames) * psf->sf.samplerate
							* ((bc.numBeats * 4.0) / bc.sigDenominator) * 60.0 ;
	psf->loop_info->root_key = bc.rootNote ;

	if (count < datasize)
		psf_binheader_readf (psf, "j", datasize - count) ;

	return 0 ;
} /* aiff_read_basc_chunk */
