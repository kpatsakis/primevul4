static double filter_generalized_cubic(const double t)
{
	const double a = -DEFAULT_FILTER_GENERALIZED_CUBIC;
	double abs_t = (double)fabs(t);
	double abs_t_sq = abs_t * abs_t;
	if (abs_t < 1) return (a + 2) * abs_t_sq * abs_t - (a + 3) * abs_t_sq + 1;
	if (abs_t < 2) return a * abs_t_sq * abs_t - 5 * a * abs_t_sq + 8 * a * abs_t - 4 * a;
	return 0;
}
