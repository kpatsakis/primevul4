static int mxf_compute_ptses_fake_index(MXFContext *mxf, MXFIndexTable *index_table)
{
    int i, j, x;
    int8_t max_temporal_offset = -128;
    uint8_t *flags;

    /* first compute how many entries we have */
    for (i = 0; i < index_table->nb_segments; i++) {
        MXFIndexTableSegment *s = index_table->segments[i];

        if (!s->nb_index_entries) {
            index_table->nb_ptses = 0;
            return 0;                               /* no TemporalOffsets */
        }

        index_table->nb_ptses += s->index_duration;
    }

    /* paranoid check */
    if (index_table->nb_ptses <= 0)
        return 0;

    if (!(index_table->ptses      = av_calloc(index_table->nb_ptses, sizeof(int64_t))) ||
        !(index_table->fake_index = av_calloc(index_table->nb_ptses, sizeof(AVIndexEntry))) ||
        !(index_table->offsets    = av_calloc(index_table->nb_ptses, sizeof(int8_t))) ||
        !(flags                   = av_calloc(index_table->nb_ptses, sizeof(uint8_t)))) {
        av_freep(&index_table->ptses);
        av_freep(&index_table->fake_index);
        av_freep(&index_table->offsets);
        return AVERROR(ENOMEM);
    }

    /* we may have a few bad TemporalOffsets
     * make sure the corresponding PTSes don't have the bogus value 0 */
    for (x = 0; x < index_table->nb_ptses; x++)
        index_table->ptses[x] = AV_NOPTS_VALUE;

    /**
     * We have this:
     *
     * x  TemporalOffset
     * 0:  0
     * 1:  1
     * 2:  1
     * 3: -2
     * 4:  1
     * 5:  1
     * 6: -2
     *
     * We want to transform it into this:
     *
     * x  DTS PTS
     * 0: -1   0
     * 1:  0   3
     * 2:  1   1
     * 3:  2   2
     * 4:  3   6
     * 5:  4   4
     * 6:  5   5
     *
     * We do this by bucket sorting x by x+TemporalOffset[x] into mxf->ptses,
     * then settings mxf->first_dts = -max(TemporalOffset[x]).
     * The latter makes DTS <= PTS.
     */
    for (i = x = 0; i < index_table->nb_segments; i++) {
        MXFIndexTableSegment *s = index_table->segments[i];
        int index_delta = 1;
        int n = s->nb_index_entries;

        if (s->nb_index_entries == 2 * s->index_duration + 1) {
            index_delta = 2;    /* Avid index */
            /* ignore the last entry - it's the size of the essence container */
            n--;
        }

        for (j = 0; j < n; j += index_delta, x++) {
            int offset = s->temporal_offset_entries[j] / index_delta;
            int index  = x + offset;

            if (x >= index_table->nb_ptses) {
                av_log(mxf->fc, AV_LOG_ERROR,
                       "x >= nb_ptses - IndexEntryCount %i < IndexDuration %"PRId64"?\n",
                       s->nb_index_entries, s->index_duration);
                break;
            }

            flags[x] = !(s->flag_entries[j] & 0x30) ? AVINDEX_KEYFRAME : 0;

            if (index < 0 || index >= index_table->nb_ptses) {
                av_log(mxf->fc, AV_LOG_ERROR,
                       "index entry %i + TemporalOffset %i = %i, which is out of bounds\n",
                       x, offset, index);
                continue;
            }

            index_table->offsets[x] = offset;
            index_table->ptses[index] = x;
            max_temporal_offset = FFMAX(max_temporal_offset, offset);
        }
    }

    /* calculate the fake index table in display order */
    for (x = 0; x < index_table->nb_ptses; x++) {
        index_table->fake_index[x].timestamp = x;
        if (index_table->ptses[x] != AV_NOPTS_VALUE)
            index_table->fake_index[index_table->ptses[x]].flags = flags[x];
    }
    av_freep(&flags);

    index_table->first_dts = -max_temporal_offset;

    return 0;
}
