static int mov_read_chap(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    unsigned i, num;
    void *new_tracks;

    num = atom.size / 4;
    if (!(new_tracks = av_malloc_array(num, sizeof(int))))
        return AVERROR(ENOMEM);

    av_free(c->chapter_tracks);
    c->chapter_tracks = new_tracks;
    c->nb_chapter_tracks = num;

    for (i = 0; i < num && !pb->eof_reached; i++)
        c->chapter_tracks[i] = avio_rb32(pb);

    return 0;
}
