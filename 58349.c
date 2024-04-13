void av_log_ask_for_sample(void *avc, const char *msg, ...)
{
    va_list argument_list;

    va_start(argument_list, msg);

    if (msg)
        av_vlog(avc, AV_LOG_WARNING, msg, argument_list);
    av_log(avc, AV_LOG_WARNING, "If you want to help, upload a sample "
            "of this file to ftp://upload.ffmpeg.org/incoming/ "
            "and contact the ffmpeg-devel mailing list. (ffmpeg-devel@ffmpeg.org)\n");

    va_end(argument_list);
}
