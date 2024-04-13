IW_IMPL(void) iw_make_gamma_csdescr(struct iw_csdescr *cs, double gamma)
{
	cs->cstype = IW_CSTYPE_GAMMA;
	cs->gamma = gamma;
	if(cs->gamma<0.1) cs->gamma=0.1;
	if(cs->gamma>10.0) cs->gamma=10.0;
	cs->srgb_intent = 0;
	optimize_csdescr(cs);
}
