static int decode_ihdr_chunk(AVCodecContext *avctx, PNGDecContext *s,
                             uint32_t length)
{
    if (length != 13)
        return AVERROR_INVALIDDATA;

    if (s->state & PNG_IDAT) {
        av_log(avctx, AV_LOG_ERROR, "IHDR after IDAT\n");
        return AVERROR_INVALIDDATA;
    }

    if (s->state & PNG_IHDR) {
        av_log(avctx, AV_LOG_ERROR, "Multiple IHDR\n");
        return AVERROR_INVALIDDATA;
    }

    s->width  = s->cur_w = bytestream2_get_be32(&s->gb);
    s->height = s->cur_h = bytestream2_get_be32(&s->gb);
    if (av_image_check_size(s->width, s->height, 0, avctx)) {
        s->cur_w = s->cur_h = s->width = s->height = 0;
        av_log(avctx, AV_LOG_ERROR, "Invalid image size\n");
        return AVERROR_INVALIDDATA;
    }
    s->bit_depth        = bytestream2_get_byte(&s->gb);
    s->color_type       = bytestream2_get_byte(&s->gb);
    s->compression_type = bytestream2_get_byte(&s->gb);
    s->filter_type      = bytestream2_get_byte(&s->gb);
    s->interlace_type   = bytestream2_get_byte(&s->gb);
    bytestream2_skip(&s->gb, 4); /* crc */
    s->state |= PNG_IHDR;
    if (avctx->debug & FF_DEBUG_PICT_INFO)
        av_log(avctx, AV_LOG_DEBUG, "width=%d height=%d depth=%d color_type=%d "
                "compression_type=%d filter_type=%d interlace_type=%d\n",
                s->width, s->height, s->bit_depth, s->color_type,
                s->compression_type, s->filter_type, s->interlace_type);

    return 0;
}
