 static void usage(char *me, bool fail)
 {
 	fprintf(stderr, "Usage: %s lxcpath name pid type bridge nicname\n", me);
	fprintf(stderr, " nicname is the name to use inside the container\n");
	exit(fail ? 1 : 0);
}
