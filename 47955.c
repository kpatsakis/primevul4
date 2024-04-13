local size_t grow(size_t size)
{
    size_t was, top;
    int shift;

    was = size;
    size += size >> 2;
    top = size;
    for (shift = 0; top > 7; shift++)
        top >>= 1;
    if (top == 7)
        size = (size_t)1 << (shift + 3);
    if (size < 16)
        size = 16;
    if (size <= was)
        size = (size_t)0 - 1;
    return size;
}
