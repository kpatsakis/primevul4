static double filter_lanczos8(const double x1)
{
	const double x = x1 < 0.0 ? -x1 : x1;
#define R DEFAULT_LANCZOS8_RADIUS

	if ( x == 0.0) return 1;

	if ( x < R) {
		return R * sin(x*M_PI) * sin(x * M_PI/ R) / (x * M_PI * x * M_PI);
	}
	return 0.0;
#undef R
}
