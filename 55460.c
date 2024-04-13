static void mov_metadata_creation_time(AVDictionary **metadata, int64_t time)
{
    if (time) {
        if(time >= 2082844800)
            time -= 2082844800;  /* seconds between 1904-01-01 and Epoch */

        if ((int64_t)(time * 1000000ULL) / 1000000 != time) {
            av_log(NULL, AV_LOG_DEBUG, "creation_time is not representable\n");
            return;
        }

        avpriv_dict_set_timestamp(metadata, "creation_time", time * 1000000);
    }
}
