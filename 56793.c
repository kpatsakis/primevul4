define_function(fsum_3)
{
  double a = float_argument(1);
  double b = float_argument(2);
  double c = float_argument(3);

  return_float(a + b + c);
}
