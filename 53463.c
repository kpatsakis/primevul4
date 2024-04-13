static double filter_hermite(const double x1)
{
	const double x = x1 < 0.0 ? -x1 : x1;

	if (x < 1.0) return ((2.0 * x - 3) * x * x + 1.0 );

	return 0.0;
}
