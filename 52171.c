cpu_util(double pcpu[3])
{
    static struct timeval last;
    static clock_t clast;
    static struct rusage rlast;
    struct timeval temp;
    clock_t ctemp;
    struct rusage rtemp;
    double timediff;
    double userdiff;
    double systemdiff;

    if (pcpu == NULL) {
        gettimeofday(&last, NULL);
        clast = clock();
	getrusage(RUSAGE_SELF, &rlast);
        return;
    }

    gettimeofday(&temp, NULL);
    ctemp = clock();
    getrusage(RUSAGE_SELF, &rtemp);

    timediff = ((temp.tv_sec * 1000000.0 + temp.tv_usec) -
                (last.tv_sec * 1000000.0 + last.tv_usec));
    userdiff = ((rtemp.ru_utime.tv_sec * 1000000.0 + rtemp.ru_utime.tv_usec) -
                (rlast.ru_utime.tv_sec * 1000000.0 + rlast.ru_utime.tv_usec));
    systemdiff = ((rtemp.ru_stime.tv_sec * 1000000.0 + rtemp.ru_stime.tv_usec) -
                  (rlast.ru_stime.tv_sec * 1000000.0 + rlast.ru_stime.tv_usec));

    pcpu[0] = (((ctemp - clast) * 1000000.0 / CLOCKS_PER_SEC) / timediff) * 100;
    pcpu[1] = (userdiff / timediff) * 100;
    pcpu[2] = (systemdiff / timediff) * 100;
}
