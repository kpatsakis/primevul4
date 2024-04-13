iprintf(struct iperf_test *test, const char* format, ...)
{
    va_list argp;
    int r = -1;

    /*
     * There are roughly two use cases here.  If we're the client,
     * want to print stuff directly to the output stream.
     * If we're the sender we might need to buffer up output to send
     * to the client.
     *
     * This doesn't make a whole lot of difference except there are
     * some chunks of output on the client (on particular the whole
     * of the server output with --get-server-output) that could
     * easily exceed the size of the line buffer, but which don't need
     * to be buffered up anyway.
     */
    if (test->role == 'c') {
	if (test->title)
	    printf("%s:  ", test->title);
	va_start(argp, format);
	r = vprintf(format, argp);
	va_end(argp);
    }
    else if (test->role == 's') {
	char linebuffer[1024];
	va_start(argp, format);
	r = vsnprintf(linebuffer, sizeof(linebuffer), format, argp);
	va_end(argp);
	printf("%s", linebuffer);

	if (test->role == 's' && iperf_get_test_get_server_output(test)) {
	    struct iperf_textline *l = (struct iperf_textline *) malloc(sizeof(struct iperf_textline));
	    l->line = strdup(linebuffer);
	    TAILQ_INSERT_TAIL(&(test->server_output_list), l, textlineentries);
	}
    }
    return r;
}
