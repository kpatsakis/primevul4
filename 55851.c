static int rm_assemble_video_frame(AVFormatContext *s, AVIOContext *pb,
                                   RMDemuxContext *rm, RMStream *vst,
                                   AVPacket *pkt, int len, int *pseq,
                                   int64_t *timestamp)
{
    int hdr;
    int seq = 0, pic_num = 0, len2 = 0, pos = 0; //init to silence compiler warning
    int type;
    int ret;

    hdr = avio_r8(pb); len--;
    type = hdr >> 6;

    if(type != 3){  // not frame as a part of packet
        seq = avio_r8(pb); len--;
    }
    if(type != 1){  // not whole frame
        len2 = get_num(pb, &len);
        pos  = get_num(pb, &len);
        pic_num = avio_r8(pb); len--;
    }
    if(len<0) {
        av_log(s, AV_LOG_ERROR, "Insufficient data\n");
        return -1;
    }
    rm->remaining_len = len;
    if(type&1){     // frame, not slice
        if(type == 3){  // frame as a part of packet
            len= len2;
            *timestamp = pos;
        }
        if(rm->remaining_len < len) {
            av_log(s, AV_LOG_ERROR, "Insufficient remaining len\n");
            return -1;
        }
        rm->remaining_len -= len;
        if(av_new_packet(pkt, len + 9) < 0)
            return AVERROR(EIO);
        pkt->data[0] = 0;
        AV_WL32(pkt->data + 1, 1);
        AV_WL32(pkt->data + 5, 0);
        if ((ret = avio_read(pb, pkt->data + 9, len)) != len) {
            av_packet_unref(pkt);
            av_log(s, AV_LOG_ERROR, "Failed to read %d bytes\n", len);
            return ret < 0 ? ret : AVERROR(EIO);
        }
        return 0;
    }

    *pseq = seq;
    if((seq & 0x7F) == 1 || vst->curpic_num != pic_num){
        if (len2 > ffio_limit(pb, len2)) {
            av_log(s, AV_LOG_ERROR, "Impossibly sized packet\n");
            return AVERROR_INVALIDDATA;
        }
        vst->slices = ((hdr & 0x3F) << 1) + 1;
        vst->videobufsize = len2 + 8*vst->slices + 1;
        av_packet_unref(&vst->pkt); //FIXME this should be output.
        if(av_new_packet(&vst->pkt, vst->videobufsize) < 0)
            return AVERROR(ENOMEM);
        memset(vst->pkt.data, 0, vst->pkt.size);
        vst->videobufpos = 8*vst->slices + 1;
        vst->cur_slice = 0;
        vst->curpic_num = pic_num;
        vst->pktpos = avio_tell(pb);
    }
    if(type == 2)
        len = FFMIN(len, pos);

    if(++vst->cur_slice > vst->slices) {
        av_log(s, AV_LOG_ERROR, "cur slice %d, too large\n", vst->cur_slice);
        return 1;
    }
    if(!vst->pkt.data)
        return AVERROR(ENOMEM);
    AV_WL32(vst->pkt.data - 7 + 8*vst->cur_slice, 1);
    AV_WL32(vst->pkt.data - 3 + 8*vst->cur_slice, vst->videobufpos - 8*vst->slices - 1);
    if(vst->videobufpos + len > vst->videobufsize) {
        av_log(s, AV_LOG_ERROR, "outside videobufsize\n");
        return 1;
    }
    if (avio_read(pb, vst->pkt.data + vst->videobufpos, len) != len)
        return AVERROR(EIO);
    vst->videobufpos += len;
    rm->remaining_len-= len;

    if (type == 2 || vst->videobufpos == vst->videobufsize) {
        vst->pkt.data[0] = vst->cur_slice-1;
        *pkt= vst->pkt;
        vst->pkt.data= NULL;
        vst->pkt.size= 0;
        vst->pkt.buf = NULL;
        if(vst->slices != vst->cur_slice) //FIXME find out how to set slices correct from the begin
            memmove(pkt->data + 1 + 8*vst->cur_slice, pkt->data + 1 + 8*vst->slices,
                vst->videobufpos - 1 - 8*vst->slices);
        pkt->size = vst->videobufpos + 8*(vst->cur_slice - vst->slices);
        pkt->pts = AV_NOPTS_VALUE;
        pkt->pos = vst->pktpos;
        vst->slices = 0;
        return 0;
    }

    return 1;
}
