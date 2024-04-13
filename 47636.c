dump_file(const char *filename)
{

    FILE *fp = NULL;
    int ch, line = 0;

    CRM_CHECK(filename != NULL, return);

    fp = fopen(filename, "r");
    CRM_CHECK(fp != NULL, return);

    fprintf(stderr, "%4d ", ++line);
    do {
        ch = getc(fp);
        if (ch == EOF) {
            putc('\n', stderr);
            break;
        } else if (ch == '\n') {
            fprintf(stderr, "\n%4d ", ++line);
        } else {
            putc(ch, stderr);
        }
    } while (1);

    fclose(fp);
}
