define_function(match)
{
  return_integer(yr_re_match(regexp_argument(1), string_argument(2)));
}
