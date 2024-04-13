iperf_catch_sigend(void (*handler)(int))
{
    signal(SIGINT, handler);
    signal(SIGTERM, handler);
    signal(SIGHUP, handler);
}
