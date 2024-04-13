local void deflate_engine(z_stream *strm, struct space *out, int flush)
{
    size_t room;

    do {
        room = out->size - out->len;
        if (room == 0) {
            grow_space(out);
            room = out->size - out->len;
        }
        strm->next_out = out->buf + out->len;
        strm->avail_out = room < UINT_MAX ? (unsigned)room : UINT_MAX;
        (void)deflate(strm, flush);
        out->len = strm->next_out - out->buf;
    } while (strm->avail_out == 0);
    assert(strm->avail_in == 0);
}
