static int mxf_read_close(AVFormatContext *s)
{
    MXFContext *mxf = s->priv_data;
    int i;

    av_freep(&mxf->packages_refs);

    for (i = 0; i < s->nb_streams; i++)
        s->streams[i]->priv_data = NULL;

    for (i = 0; i < mxf->metadata_sets_count; i++) {
        mxf_free_metadataset(mxf->metadata_sets + i, 1);
    }
    av_freep(&mxf->partitions);
    av_freep(&mxf->metadata_sets);
    av_freep(&mxf->aesc);
    av_freep(&mxf->local_tags);

    if (mxf->index_tables) {
        for (i = 0; i < mxf->nb_index_tables; i++) {
            av_freep(&mxf->index_tables[i].segments);
            av_freep(&mxf->index_tables[i].ptses);
            av_freep(&mxf->index_tables[i].fake_index);
            av_freep(&mxf->index_tables[i].offsets);
        }
    }
    av_freep(&mxf->index_tables);

    return 0;
}
