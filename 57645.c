IW_IMPL(void) iw_make_srgb_csdescr_2(struct iw_csdescr *cs)
{
	cs->cstype = IW_CSTYPE_SRGB;
	cs->gamma = 0.0;
}
