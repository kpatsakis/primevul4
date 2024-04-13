static int klv_read_packet(KLVPacket *klv, AVIOContext *pb)
{
    if (!mxf_read_sync(pb, mxf_klv_key, 4))
        return AVERROR_INVALIDDATA;
    klv->offset = avio_tell(pb) - 4;
    memcpy(klv->key, mxf_klv_key, 4);
    avio_read(pb, klv->key + 4, 12);
    klv->length = klv_decode_ber_length(pb);
    return klv->length == -1 ? -1 : 0;
}
