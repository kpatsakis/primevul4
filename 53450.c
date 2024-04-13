static double filter_bicubic(const double t)
{
	const double abs_t = (double)fabs(t);
	const double abs_t_sq = abs_t * abs_t;
	if (abs_t<1) return 1-2*abs_t_sq+abs_t_sq*abs_t;
	if (abs_t<2) return 4 - 8*abs_t +5*abs_t_sq - abs_t_sq*abs_t;
	return 0;
}
