static double filter_bessel(const double x)
{
	if (x == 0.0f)
		return (double)(M_PI/4.0f);
	return (KernelBessel_Order1((double)M_PI*x)/(2.0f*x));
}
