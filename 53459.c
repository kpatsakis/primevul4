static double filter_gaussian(const double x)
{
	/* return(exp((double) (-2.0 * x * x)) * sqrt(2.0 / M_PI)); */
	return (double)(exp(-2.0f * x * x) * 0.79788456080287f);
}
