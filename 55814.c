static int read_from_url(struct playlist *pls, struct segment *seg,
                         uint8_t *buf, int buf_size,
                         enum ReadFromURLMode mode)
{
    int ret;

     /* limit read if the segment was only a part of a file */
    if (seg->size >= 0)
        buf_size = FFMIN(buf_size, seg->size - pls->cur_seg_offset);

    if (mode == READ_COMPLETE) {
        ret = avio_read(pls->input, buf, buf_size);
        if (ret != buf_size)
            av_log(NULL, AV_LOG_ERROR, "Could not read complete segment.\n");
    } else
        ret = avio_read(pls->input, buf, buf_size);

    if (ret > 0)
        pls->cur_seg_offset += ret;

    return ret;
}
