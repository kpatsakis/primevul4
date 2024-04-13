static void intercept_id3(struct playlist *pls, uint8_t *buf,
                         int buf_size, int *len)
{
    /* intercept id3 tags, we do not want to pass them to the raw
     * demuxer on all segment switches */
    int bytes;
    int id3_buf_pos = 0;
    int fill_buf = 0;
    struct segment *seg = current_segment(pls);

    /* gather all the id3 tags */
    while (1) {
        /* see if we can retrieve enough data for ID3 header */
        if (*len < ID3v2_HEADER_SIZE && buf_size >= ID3v2_HEADER_SIZE) {
            bytes = read_from_url(pls, seg, buf + *len, ID3v2_HEADER_SIZE - *len, READ_COMPLETE);
            if (bytes > 0) {

                if (bytes == ID3v2_HEADER_SIZE - *len)
                    /* no EOF yet, so fill the caller buffer again after
                     * we have stripped the ID3 tags */
                    fill_buf = 1;

                *len += bytes;

            } else if (*len <= 0) {
                /* error/EOF */
                *len = bytes;
                fill_buf = 0;
            }
        }

        if (*len < ID3v2_HEADER_SIZE)
            break;

        if (ff_id3v2_match(buf, ID3v2_DEFAULT_MAGIC)) {
            int64_t maxsize = seg->size >= 0 ? seg->size : 1024*1024;
            int taglen = ff_id3v2_tag_len(buf);
            int tag_got_bytes = FFMIN(taglen, *len);
            int remaining = taglen - tag_got_bytes;

            if (taglen > maxsize) {
                av_log(pls->ctx, AV_LOG_ERROR, "Too large HLS ID3 tag (%d > %"PRId64" bytes)\n",
                       taglen, maxsize);
                break;
            }

            /*
             * Copy the id3 tag to our temporary id3 buffer.
             * We could read a small id3 tag directly without memcpy, but
             * we would still need to copy the large tags, and handling
             * both of those cases together with the possibility for multiple
             * tags would make the handling a bit complex.
             */
            pls->id3_buf = av_fast_realloc(pls->id3_buf, &pls->id3_buf_size, id3_buf_pos + taglen);
            if (!pls->id3_buf)
                break;
            memcpy(pls->id3_buf + id3_buf_pos, buf, tag_got_bytes);
            id3_buf_pos += tag_got_bytes;

            /* strip the intercepted bytes */
            *len -= tag_got_bytes;
            memmove(buf, buf + tag_got_bytes, *len);
            av_log(pls->ctx, AV_LOG_DEBUG, "Stripped %d HLS ID3 bytes\n", tag_got_bytes);

            if (remaining > 0) {
                /* read the rest of the tag in */
                if (read_from_url(pls, seg, pls->id3_buf + id3_buf_pos, remaining, READ_COMPLETE) != remaining)
                    break;
                id3_buf_pos += remaining;
                av_log(pls->ctx, AV_LOG_DEBUG, "Stripped additional %d HLS ID3 bytes\n", remaining);
            }

        } else {
            /* no more ID3 tags */
            break;
        }
    }

    /* re-fill buffer for the caller unless EOF */
    if (*len >= 0 && (fill_buf || *len == 0)) {
        bytes = read_from_url(pls, seg, buf + *len, buf_size - *len, READ_NORMAL);

        /* ignore error if we already had some data */
        if (bytes >= 0)
            *len += bytes;
        else if (*len == 0)
            *len = bytes;
    }

    if (pls->id3_buf) {
        /* Now parse all the ID3 tags */
        AVIOContext id3ioctx;
        ffio_init_context(&id3ioctx, pls->id3_buf, id3_buf_pos, 0, NULL, NULL, NULL, NULL);
        handle_id3(&id3ioctx, pls);
    }

    if (pls->is_id3_timestamped == -1)
        pls->is_id3_timestamped = (pls->id3_mpegts_timestamp != AV_NOPTS_VALUE);
}
