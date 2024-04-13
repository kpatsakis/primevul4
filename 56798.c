define_function(foobar)
{
  int64_t arg = integer_argument(1);

  switch (arg)
  {
    case 1:
      return_string("foo");
      break;
    case 2:
      return_string("bar");
      break;
  }

  return_string("oops")
}
