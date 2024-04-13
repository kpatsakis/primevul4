int postgresTimeStampForTimeString(const char *timestring, char *dest, size_t destsize)
{
  int nlength = strlen(timestring);
  int timeresolution = msTimeGetResolution(timestring);
  int bNoDate = (*timestring == 'T');
  if (timeresolution < 0)
    return MS_FALSE;

  switch(timeresolution) {
    case TIME_RESOLUTION_YEAR:
      if (timestring[nlength-1] != '-') {
        snprintf(dest, destsize,"date '%s-01-01'",timestring);
      } else {
        snprintf(dest, destsize,"date '%s01-01'",timestring);
      }
      break;
    case TIME_RESOLUTION_MONTH:
      if (timestring[nlength-1] != '-') {
        snprintf(dest, destsize,"date '%s-01'",timestring);
      } else {
        snprintf(dest, destsize,"date '%s01'",timestring);
      }
      break;
    case TIME_RESOLUTION_DAY:
      snprintf(dest, destsize,"date '%s'",timestring);
      break;
    case TIME_RESOLUTION_HOUR:
      if (timestring[nlength-1] != ':') {
        if(bNoDate)
          snprintf(dest, destsize,"time '%s:00:00'", timestring);
        else
          snprintf(dest, destsize,"timestamp '%s:00:00'", timestring);
      } else {
        if(bNoDate)
          snprintf(dest, destsize,"time '%s00:00'", timestring);
        else
          snprintf(dest, destsize,"timestamp '%s00:00'", timestring);
      }
      break;
    case TIME_RESOLUTION_MINUTE:
      if (timestring[nlength-1] != ':') {
        if(bNoDate)
          snprintf(dest, destsize,"time '%s:00'", timestring);
        else
          snprintf(dest, destsize,"timestamp '%s:00'", timestring);
      } else {
        if(bNoDate)
          snprintf(dest, destsize,"time '%s00'", timestring);
        else
          snprintf(dest, destsize,"timestamp '%s00'", timestring);
      }
      break;
    case TIME_RESOLUTION_SECOND:
      if(bNoDate)
         snprintf(dest, destsize,"time '%s'", timestring);
      else
         snprintf(dest, destsize,"timestamp '%s'", timestring);
      break;
    default:
      return MS_FAILURE;
  }
  return MS_SUCCESS;

}
