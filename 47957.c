local void help(void)
{
    int n;

    if (g.verbosity == 0)
        return;
    for (n = 0; n < (int)(sizeof(helptext) / sizeof(char *)); n++)
        fprintf(stderr, "%s\n", helptext[n]);
    fflush(stderr);
    exit(0);
}
