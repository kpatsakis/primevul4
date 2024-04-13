static int mov_read_timecode_track(AVFormatContext *s, AVStream *st)
{
    MOVStreamContext *sc = st->priv_data;
    int flags = 0;
    int64_t cur_pos = avio_tell(sc->pb);
    uint32_t value;

    if (!st->nb_index_entries)
        return -1;

    avio_seek(sc->pb, st->index_entries->pos, SEEK_SET);
    value = avio_rb32(s->pb);

    if (sc->tmcd_flags & 0x0001) flags |= AV_TIMECODE_FLAG_DROPFRAME;
    if (sc->tmcd_flags & 0x0002) flags |= AV_TIMECODE_FLAG_24HOURSMAX;
    if (sc->tmcd_flags & 0x0004) flags |= AV_TIMECODE_FLAG_ALLOWNEGATIVE;

    /* Assume Counter flag is set to 1 in tmcd track (even though it is likely
     * not the case) and thus assume "frame number format" instead of QT one.
     * No sample with tmcd track can be found with a QT timecode at the moment,
     * despite what the tmcd track "suggests" (Counter flag set to 0 means QT
     * format). */
    parse_timecode_in_framenum_format(s, st, value, flags);

    avio_seek(sc->pb, cur_pos, SEEK_SET);
    return 0;
}
