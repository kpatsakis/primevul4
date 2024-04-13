static double filter_blackman(const double x)
{
	return (0.42f+0.5f*(double)cos(M_PI*x)+0.08f*(double)cos(2.0f*M_PI*x));
}
