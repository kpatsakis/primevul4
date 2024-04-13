static void parse_profile_level_id(AVFormatContext *s,
                                   PayloadContext *h264_data,
                                   const char *value)
{
    char buffer[3];
    uint8_t profile_idc;
    uint8_t profile_iop;
    uint8_t level_idc;

    buffer[0]   = value[0];
    buffer[1]   = value[1];
    buffer[2]   = '\0';
    profile_idc = strtol(buffer, NULL, 16);
    buffer[0]   = value[2];
    buffer[1]   = value[3];
    profile_iop = strtol(buffer, NULL, 16);
    buffer[0]   = value[4];
    buffer[1]   = value[5];
    level_idc   = strtol(buffer, NULL, 16);

    av_log(s, AV_LOG_DEBUG,
           "RTP Profile IDC: %x Profile IOP: %x Level: %x\n",
           profile_idc, profile_iop, level_idc);
    h264_data->profile_idc = profile_idc;
    h264_data->profile_iop = profile_iop;
    h264_data->level_idc   = level_idc;
}
