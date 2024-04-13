YR_OBJECT* yr_object_lookup(
    YR_OBJECT* object,
    int flags,
    const char* pattern,
    ...)
{
  YR_OBJECT* result;

  va_list args;
  va_start(args, pattern);

  result = _yr_object_lookup(object, flags, pattern, args);

  va_end(args);

  return result;

}
