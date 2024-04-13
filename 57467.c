int _yr_scan_icompare(
    uint8_t* data,
    size_t data_size,
    uint8_t* string,
    size_t string_length)
{
  uint8_t* s1 = data;
  uint8_t* s2 = string;

  size_t i = 0;

  if (data_size < string_length)
    return 0;

  while (i < string_length && yr_lowercase[*s1++] == yr_lowercase[*s2++])
    i++;

  return (int) ((i == string_length) ? i : 0);
}
