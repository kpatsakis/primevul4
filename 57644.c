IW_IMPL(void) iw_make_rec709_csdescr(struct iw_csdescr *cs)
{
	cs->cstype = IW_CSTYPE_REC709;
	cs->gamma = 0.0;
}
