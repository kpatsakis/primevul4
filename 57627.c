IW_IMPL(unsigned int) iw_color_get_int_sample(struct iw_color *clr, int channel,
	unsigned int maxcolorcode)
{
	int n;
	n = (int)(0.5+(clr->c[channel] * (double)maxcolorcode));
	if(n<0) n=0;
	else if(n>(int)maxcolorcode) n=(int)maxcolorcode;
	return (unsigned int)n;
}
