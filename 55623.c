static void mxf_handle_small_eubc(AVFormatContext *s)
{
    MXFContext *mxf = s->priv_data;

    /* assuming non-OPAtom == frame wrapped
     * no sane writer would wrap 2 byte PCM packets with 20 byte headers.. */
    AVStream *st = mxf_get_opatom_stream(mxf);
    if (!st)
        return;

    /* expect PCM with exactly one index table segment and a small (< 32) EUBC */
    if (st->codecpar->codec_type != AVMEDIA_TYPE_AUDIO         ||
        !is_pcm(st->codecpar->codec_id)                        ||
        mxf->nb_index_tables != 1                              ||
        mxf->index_tables[0].nb_segments != 1                  ||
        mxf->index_tables[0].segments[0]->edit_unit_byte_count >= 32)
        return;

    /* arbitrarily default to 48 kHz PAL audio frame size */
    /* TODO: We could compute this from the ratio between the audio
     *       and video edit rates for 48 kHz NTSC we could use the
     *       1802-1802-1802-1802-1801 pattern. */
    mxf->edit_units_per_packet = 1920;
}
