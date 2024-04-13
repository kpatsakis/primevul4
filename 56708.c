void ff_rtmp_packet_dump(void *ctx, RTMPPacket *p)
{
    av_log(ctx, AV_LOG_DEBUG, "RTMP packet type '%s'(%d) for channel %d, timestamp %d, extra field %d size %d\n",
           rtmp_packet_type(p->type), p->type, p->channel_id, p->timestamp, p->extra, p->size);
    if (p->type == RTMP_PT_INVOKE || p->type == RTMP_PT_NOTIFY) {
        uint8_t *src = p->data, *src_end = p->data + p->size;
        while (src < src_end) {
            int sz;
            amf_tag_contents(ctx, src, src_end);
            sz = ff_amf_tag_size(src, src_end);
            if (sz < 0)
                break;
            src += sz;
        }
    } else if (p->type == RTMP_PT_SERVER_BW){
        av_log(ctx, AV_LOG_DEBUG, "Server BW = %d\n", AV_RB32(p->data));
    } else if (p->type == RTMP_PT_CLIENT_BW){
        av_log(ctx, AV_LOG_DEBUG, "Client BW = %d\n", AV_RB32(p->data));
    } else if (p->type != RTMP_PT_AUDIO && p->type != RTMP_PT_VIDEO && p->type != RTMP_PT_METADATA) {
        int i;
        for (i = 0; i < p->size; i++)
            av_log(ctx, AV_LOG_DEBUG, " %02X", p->data[i]);
        av_log(ctx, AV_LOG_DEBUG, "\n");
    }
}
