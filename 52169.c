usage_long()
{
    fprintf(stderr, usage_longstr, UDP_RATE / (1024*1024), DURATION, DEFAULT_TCP_BLKSIZE / 1024, DEFAULT_UDP_BLKSIZE / 1024);
}
