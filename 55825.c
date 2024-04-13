static int rl2_read_packet(AVFormatContext *s,
                            AVPacket *pkt)
{
    Rl2DemuxContext *rl2 = s->priv_data;
    AVIOContext *pb = s->pb;
    AVIndexEntry *sample = NULL;
    int i;
    int ret = 0;
    int stream_id = -1;
    int64_t pos = INT64_MAX;

    /** check if there is a valid video or audio entry that can be used */
    for(i=0; i<s->nb_streams; i++){
        if(rl2->index_pos[i] < s->streams[i]->nb_index_entries
              && s->streams[i]->index_entries[ rl2->index_pos[i] ].pos < pos){
            sample = &s->streams[i]->index_entries[ rl2->index_pos[i] ];
            pos= sample->pos;
            stream_id= i;
        }
    }

    if(stream_id == -1)
        return AVERROR_EOF;

    ++rl2->index_pos[stream_id];

    /** position the stream (will probably be there anyway) */
    avio_seek(pb, sample->pos, SEEK_SET);

    /** fill the packet */
    ret = av_get_packet(pb, pkt, sample->size);
    if(ret != sample->size){
        av_packet_unref(pkt);
        return AVERROR(EIO);
    }

    pkt->stream_index = stream_id;
    pkt->pts = sample->timestamp;

    return ret;
}
