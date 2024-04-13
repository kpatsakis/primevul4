static int get_stream_idx(const unsigned *d)
{
    if (d[0] >= '0' && d[0] <= '9' &&
        d[1] >= '0' && d[1] <= '9') {
        return (d[0] - '0') * 10 + (d[1] - '0');
    } else {
        return 100; // invalid stream ID
    }
}
