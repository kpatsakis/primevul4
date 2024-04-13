static double filter_cubic_spline(const double x1)
{
	const double x = x1 < 0.0 ? -x1 : x1;

	if (x < 1.0 ) {
		const double x2 = x*x;

		return (0.5 * x2 * x - x2 + 2.0 / 3.0);
	}
	if (x < 2.0) {
		return (pow(2.0 - x, 3.0)/6.0);
	}
	return 0;
}
