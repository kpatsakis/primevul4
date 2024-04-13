static double filter_quadratic_bspline(const double x1)
{
	const double x = x1 < 0.0 ? -x1 : x1;

	if (x <= 0.5) return (- x * x + 0.75);
	if (x <= 1.5) return (0.5 * x * x - 1.5 * x + 1.125);
	return 0.0;
}
