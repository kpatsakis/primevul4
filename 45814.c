int createPostgresTimeCompareRange(const char *timecol, const char *mintime, const char *maxtime,
                                   char *dest, size_t destsize)
{
  int mintimeresolution = msTimeGetResolution(mintime);
  int maxtimeresolution = msTimeGetResolution(maxtime);
  char minTimeStamp[100];
  char maxTimeStamp[100];
  char *minTimeInterval,*maxTimeInterval;
  if (mintimeresolution < 0 || maxtimeresolution < 0)
    return MS_FALSE;
  postgresTimeStampForTimeString(mintime,minTimeStamp,100);
  postgresTimeStampForTimeString(maxtime,maxTimeStamp,100);

  switch(maxtimeresolution) {
    case TIME_RESOLUTION_YEAR:
      maxTimeInterval = "year";
      break;
    case TIME_RESOLUTION_MONTH:
      maxTimeInterval = "month";
      break;
    case TIME_RESOLUTION_DAY:
      maxTimeInterval = "day";
      break;
    case TIME_RESOLUTION_HOUR:
      maxTimeInterval = "hour";
      break;
    case TIME_RESOLUTION_MINUTE:
      maxTimeInterval = "minute";
      break;
    case TIME_RESOLUTION_SECOND:
      maxTimeInterval = "second";
      break;
    default:
      return MS_FAILURE;
  }
  switch(mintimeresolution) {
    case TIME_RESOLUTION_YEAR:
      minTimeInterval = "year";
      break;
    case TIME_RESOLUTION_MONTH:
      minTimeInterval = "month";
      break;
    case TIME_RESOLUTION_DAY:
      minTimeInterval = "day";
      break;
    case TIME_RESOLUTION_HOUR:
      minTimeInterval = "hour";
      break;
    case TIME_RESOLUTION_MINUTE:
      minTimeInterval = "minute";
      break;
    case TIME_RESOLUTION_SECOND:
      minTimeInterval = "second";
      break;
    default:
      return MS_FAILURE;
  }
  snprintf(dest, destsize,"(%s >= date_trunc('%s',%s) and %s < date_trunc('%s',%s) + interval '1 %s')",
           timecol, minTimeInterval, minTimeStamp,
           timecol, maxTimeInterval, maxTimeStamp, maxTimeInterval);
  return MS_SUCCESS;
}
