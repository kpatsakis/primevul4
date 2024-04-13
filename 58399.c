int ff_alloc_a53_sei(const AVFrame *frame, size_t prefix_len,
                     void **data, size_t *sei_size)
{
    AVFrameSideData *side_data = NULL;
    uint8_t *sei_data;

    if (frame)
        side_data = av_frame_get_side_data(frame, AV_FRAME_DATA_A53_CC);

    if (!side_data) {
        *data = NULL;
        return 0;
    }

    *sei_size = side_data->size + 11;
    *data = av_mallocz(*sei_size + prefix_len);
    if (!*data)
        return AVERROR(ENOMEM);
    sei_data = (uint8_t*)*data + prefix_len;

    sei_data[0] = 181;
    sei_data[1] = 0;
    sei_data[2] = 49;

    /**
     * 'GA94' is standard in North America for ATSC, but hard coding
     * this style may not be the right thing to do -- other formats
     * do exist. This information is not available in the side_data
     * so we are going with this right now.
     */
    AV_WL32(sei_data + 3, MKTAG('G', 'A', '9', '4'));
    sei_data[7] = 3;
    sei_data[8] = ((side_data->size/3) & 0x1f) | 0x40;
    sei_data[9] = 0;

    memcpy(sei_data + 10, side_data->data, side_data->size);

    sei_data[side_data->size+10] = 255;

    return 0;
}
