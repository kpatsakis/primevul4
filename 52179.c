timeval_to_double(struct timeval * tv)
{
    double d;

    d = tv->tv_sec + tv->tv_usec / 1000000;

    return d;
}
