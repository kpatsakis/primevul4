static int readfull(AVFormatContext *s, AVIOContext *pb, uint8_t *dst, int n) {
    int ret = avio_read(pb, dst, n);
    if (ret != n) {
        if (ret >= 0) memset(dst + ret, 0, n - ret);
        else          memset(dst      , 0, n);
        av_log(s, AV_LOG_ERROR, "Failed to fully read block\n");
    }
    return ret;
}
