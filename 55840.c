RMStream *ff_rm_alloc_rmstream (void)
{
    RMStream *rms = av_mallocz(sizeof(RMStream));
    if (!rms)
        return NULL;
    rms->curpic_num = -1;
    return rms;
}
