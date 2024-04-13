static void mov_update_dts_shift(MOVStreamContext *sc, int duration)
{
    if (duration < 0) {
        if (duration == INT_MIN) {
            av_log(NULL, AV_LOG_WARNING, "mov_update_dts_shift(): dts_shift set to %d\n", INT_MAX);
            duration++;
        }
        sc->dts_shift = FFMAX(sc->dts_shift, -duration);
    }
}
