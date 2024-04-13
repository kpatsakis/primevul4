static int add_metadata_from_side_data(AVPacket *avpkt, AVFrame *frame)
{
    int size;
    const uint8_t *side_metadata;

    AVDictionary **frame_md = avpriv_frame_get_metadatap(frame);

    side_metadata = av_packet_get_side_data(avpkt,
                                            AV_PKT_DATA_STRINGS_METADATA, &size);
    return av_packet_unpack_dictionary(side_metadata, size, frame_md);
}
