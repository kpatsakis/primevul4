static int64_t mxf_timestamp_to_int64(uint64_t timestamp)
{
    struct tm time = { 0 };
    time.tm_year = (timestamp >> 48) - 1900;
    time.tm_mon  = (timestamp >> 40 & 0xFF) - 1;
    time.tm_mday = (timestamp >> 32 & 0xFF);
    time.tm_hour = (timestamp >> 24 & 0xFF);
    time.tm_min  = (timestamp >> 16 & 0xFF);
    time.tm_sec  = (timestamp >> 8  & 0xFF);

    /* msvcrt versions of strftime calls the invalid parameter handler
     * (aborting the process if one isn't set) if the parameters are out
     * of range. */
    time.tm_mon  = av_clip(time.tm_mon,  0, 11);
    time.tm_mday = av_clip(time.tm_mday, 1, 31);
    time.tm_hour = av_clip(time.tm_hour, 0, 23);
    time.tm_min  = av_clip(time.tm_min,  0, 59);
    time.tm_sec  = av_clip(time.tm_sec,  0, 59);

    return (int64_t)av_timegm(&time) * 1000000;
}
