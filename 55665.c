static int mxf_set_audio_pts(MXFContext *mxf, AVCodecParameters *par,
                             AVPacket *pkt)
{
    MXFTrack *track = mxf->fc->streams[pkt->stream_index]->priv_data;
    int64_t bits_per_sample = par->bits_per_coded_sample;

    if (!bits_per_sample)
        bits_per_sample = av_get_bits_per_sample(par->codec_id);

    pkt->pts = track->sample_count;

    if (   par->channels <= 0
        || bits_per_sample <= 0
        || par->channels * (int64_t)bits_per_sample < 8)
        return AVERROR(EINVAL);
    track->sample_count += pkt->size / (par->channels * (int64_t)bits_per_sample / 8);
    return 0;
}
