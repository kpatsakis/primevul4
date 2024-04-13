static wchar_t *ConvertUTF8ToUTF16(const unsigned char *source,size_t *length)
{
  wchar_t
    *utf16;

  *length=UTF8ToUTF16(source,(wchar_t *) NULL);
  if (*length == 0)
    {
      register ssize_t
        i;

      /*
        Not UTF-8, just copy.
      */
      *length=strlen((const char *) source);
      utf16=(wchar_t *) AcquireQuantumMemory(*length+1,sizeof(*utf16));
      if (utf16 == (wchar_t *) NULL)
        return((wchar_t *) NULL);
      for (i=0; i <= (ssize_t) *length; i++)
        utf16[i]=source[i];
      return(utf16);
    }
  utf16=(wchar_t *) AcquireQuantumMemory(*length+1,sizeof(*utf16));
  if (utf16 == (wchar_t *) NULL)
    return((wchar_t *) NULL);
  *length=UTF8ToUTF16(source,utf16);
  return(utf16);
}
