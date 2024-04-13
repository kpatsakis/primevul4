static double filter_cosine(const double x)
{
	if ((x >= -1.0) && (x <= 1.0)) return ((cos(x * M_PI) + 1.0)/2.0);

	return 0;
}
