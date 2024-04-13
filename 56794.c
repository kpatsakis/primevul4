define_function(isum_2)
{
  int64_t a = integer_argument(1);
  int64_t b = integer_argument(2);

  return_integer(a + b);
}
