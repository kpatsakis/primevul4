getcwd_nothrow (char *buf, size_t size)
{
  char *result;

  TRY_MSVC_INVAL
    {
      result = _getcwd (buf, size);
    }
  CATCH_MSVC_INVAL
    {
      result = NULL;
      errno = ERANGE;
    }
  DONE_MSVC_INVAL;

  return result;
}