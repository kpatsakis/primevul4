int ff_lock_avcodec(AVCodecContext *log_ctx, const AVCodec *codec)
{
    if (codec->caps_internal & FF_CODEC_CAP_INIT_THREADSAFE || !codec->init)
        return 0;

    if (lockmgr_cb) {
        if ((*lockmgr_cb)(&codec_mutex, AV_LOCK_OBTAIN))
            return -1;
    }

    if (avpriv_atomic_int_add_and_fetch(&entangled_thread_counter, 1) != 1) {
        av_log(log_ctx, AV_LOG_ERROR,
               "Insufficient thread locking. At least %d threads are "
               "calling avcodec_open2() at the same time right now.\n",
               entangled_thread_counter);
        if (!lockmgr_cb)
            av_log(log_ctx, AV_LOG_ERROR, "No lock manager is set, please see av_lockmgr_register()\n");
        ff_avcodec_locked = 1;
        ff_unlock_avcodec(codec);
        return AVERROR(EINVAL);
    }
    av_assert0(!ff_avcodec_locked);
    ff_avcodec_locked = 1;
    return 0;
}
