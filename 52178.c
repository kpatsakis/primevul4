timeval_equals(struct timeval * tv0, struct timeval * tv1)
{
    if ( tv0->tv_sec == tv1->tv_sec && tv0->tv_usec == tv1->tv_usec )
	return 1;
    else
	return 0;
}
