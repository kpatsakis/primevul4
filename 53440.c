static double KernelBessel_Order1(double x)
{
	double p, q;
	
	if (x == 0.0)
		return (0.0f);
	p = x;
	if (x < 0.0)
		x=(-x);
	if (x < 8.0)
		return (p*KernelBessel_J1(x));
	q = (double)sqrt(2.0f/(M_PI*x))*(double)(KernelBessel_P1(x)*(1.0f/sqrt(2.0f)*(sin(x)-cos(x)))-8.0f/x*KernelBessel_Q1(x)*
		(-1.0f/sqrt(2.0f)*(sin(x)+cos(x))));
	if (p < 0.0f)
		q = (-q);
	return (q);
}
