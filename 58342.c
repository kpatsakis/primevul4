int av_get_audio_frame_duration2(AVCodecParameters *par, int frame_bytes)
{
    return get_audio_frame_duration(par->codec_id, par->sample_rate,
                                    par->channels, par->block_align,
                                    par->codec_tag, par->bits_per_coded_sample,
                                    par->bit_rate, par->extradata, par->frame_size,
                                    frame_bytes);
}
