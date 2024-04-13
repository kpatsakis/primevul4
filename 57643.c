IW_IMPL(void) iw_make_linear_csdescr(struct iw_csdescr *cs)
{
	cs->cstype = IW_CSTYPE_LINEAR;
	cs->gamma = 0.0;
	cs->srgb_intent = 0;
}
