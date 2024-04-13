void av_log_missing_feature(void *avc, const char *feature, int want_sample)
{
    av_log(avc, AV_LOG_WARNING, "%s is not implemented. Update your FFmpeg "
            "version to the newest one from Git. If the problem still "
            "occurs, it means that your file has a feature which has not "
            "been implemented.\n", feature);
    if(want_sample)
        av_log_ask_for_sample(avc, NULL);
}
