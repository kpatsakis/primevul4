static void write_tIME_chunk(Image *image,png_struct *ping,png_info *info,
  const char *date,ExceptionInfo *exception)
{
  unsigned int
    day,
    hour,
    minute,
    month,
    second,
    year;

  png_time
    ptime;

  time_t
    ttime;

  if (date != (const char *) NULL)
    {
      if (sscanf(date,"%d-%d-%dT%d:%d:%dZ",&year,&month,&day,&hour,&minute,
          &second) != 6)
        {
          (void) ThrowMagickException(exception,GetMagickModule(),CoderError,
            "Invalid date format specified for png:tIME","`%s'",
            image->filename);
          return;
        }
      ptime.year=(png_uint_16) year;
      ptime.month=(png_byte) month;
      ptime.day=(png_byte) day;
      ptime.hour=(png_byte) hour;
      ptime.minute=(png_byte) minute;
      ptime.second=(png_byte) second;
    }
  else
  {
    time(&ttime);
    png_convert_from_time_t(&ptime,ttime);
  }
  png_set_tIME(ping,info,&ptime);
}
