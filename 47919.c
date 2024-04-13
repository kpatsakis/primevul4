static void usage(const char *me)
{
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "%s mountpoint\n", me);
	fprintf(stderr, "%s -h\n", me);
	exit(1);
}
