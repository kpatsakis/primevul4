define_function(isum_3)
{
  int64_t a = integer_argument(1);
  int64_t b = integer_argument(2);
  int64_t c = integer_argument(3);

  return_integer(a + b + c);
}
