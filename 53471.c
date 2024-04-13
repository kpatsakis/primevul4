static double filter_triangle(const double x1)
{
	const double x = x1 < 0.0 ? -x1 : x1;
	if (x < 1.0) return (1.0 - x);
	return 0.0;
}
