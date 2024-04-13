static int should_retry(AVIOContext *pb, int error_code) {
    if (error_code == AVERROR_EOF || avio_feof(pb))
        return 0;

    return 1;
}
