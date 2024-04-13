static double filter_power(const double x)
{
	const double a = 2.0f;
	if (fabs(x)>1) return 0.0f;
	return (1.0f - (double)fabs(pow(x,a)));
}
