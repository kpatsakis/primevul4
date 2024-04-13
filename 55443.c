static int64_t add_index_entry(AVStream *st, int64_t pos, int64_t timestamp,
                               int size, int distance, int flags)
{
    AVIndexEntry *entries, *ie;
    int64_t index = -1;
    const size_t min_size_needed = (st->nb_index_entries + 1) * sizeof(AVIndexEntry);

    const size_t requested_size =
        min_size_needed > st->index_entries_allocated_size ?
        FFMAX(min_size_needed, 2 * st->index_entries_allocated_size) :
        min_size_needed;

    if((unsigned)st->nb_index_entries + 1 >= UINT_MAX / sizeof(AVIndexEntry))
        return -1;

    entries = av_fast_realloc(st->index_entries,
                              &st->index_entries_allocated_size,
                              requested_size);
    if(!entries)
        return -1;

    st->index_entries= entries;

    index= st->nb_index_entries++;
    ie= &entries[index];

    ie->pos = pos;
    ie->timestamp = timestamp;
    ie->min_distance= distance;
    ie->size= size;
    ie->flags = flags;
    return index;
}
