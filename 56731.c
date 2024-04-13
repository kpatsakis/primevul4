static void read_tIME_chunk(Image *image,png_struct *ping,png_info *info,
  ExceptionInfo *exception)
{
  png_timep
    time;

  if (png_get_tIME(ping,info,&time))
    {
      char
        timestamp[21];

      FormatLocaleString(timestamp,21,"%04d-%02d-%02dT%02d:%02d:%02dZ",
        time->year,time->month,time->day,time->hour,time->minute,time->second);
      SetImageProperty(image,"png:tIME",timestamp,exception);
    }
}
