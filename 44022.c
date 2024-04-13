showversion(argv)
    char **argv;
{
    if (phase == PHASE_INITIALIZE) {
	fprintf(stderr, "pppd version %s\n", VERSION);
	exit(0);
    }
    return 0;
}
