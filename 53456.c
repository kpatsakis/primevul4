static double filter_cubic_convolution(const double x1)
{
	const double x = x1 < 0.0 ? -x1 : x1;
	const double x2 = x1 * x1;
	const double x2_x = x2 * x;

	if (x <= 1.0) return ((4.0 / 3.0)* x2_x - (7.0 / 3.0) * x2 + 1.0);
	if (x <= 2.0) return (- (7.0 / 12.0) * x2_x + 3 * x2 - (59.0 / 12.0) * x + 2.5);
	if (x <= 3.0) return ( (1.0/12.0) * x2_x - (2.0 / 3.0) * x2 + 1.75 * x - 1.5);
	return 0;
}
