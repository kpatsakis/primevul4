static int64_t find_prev_closest_index(AVStream *st,
                                       AVIndexEntry *e_old,
                                       int nb_old,
                                       int64_t timestamp,
                                       int flag)
{
    AVIndexEntry *e_keep = st->index_entries;
    int nb_keep = st->nb_index_entries;
    int64_t found = -1;
    int64_t i = 0;

    st->index_entries = e_old;
    st->nb_index_entries = nb_old;
    found = av_index_search_timestamp(st, timestamp, flag | AVSEEK_FLAG_BACKWARD);

    if (found >= 0) {
        for (i = found; i > 0 && e_old[i].timestamp == e_old[i - 1].timestamp;
             i--) {
            if ((flag & AVSEEK_FLAG_ANY) ||
                (e_old[i - 1].flags & AVINDEX_KEYFRAME)) {
                found = i - 1;
            }
        }
    }

    /* restore AVStream state*/
    st->index_entries = e_keep;
    st->nb_index_entries = nb_keep;
    return found;
}
