static int get_consumed_bytes(int pos, int buf_size)
{
    if (pos == 0)
        pos = 1;        // avoid infinite loops (I doubt that is needed but...)
    if (pos + 10 > buf_size)
        pos = buf_size; // oops ;)

    return pos;
}
