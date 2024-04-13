static void optimize_csdescr(struct iw_csdescr *cs)
{
	if(cs->cstype!=IW_CSTYPE_GAMMA) return;
	if(cs->gamma>=0.999995 && cs->gamma<=1.000005) {
		cs->cstype = IW_CSTYPE_LINEAR;
	}
}
