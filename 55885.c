static unsigned char readuchar(FILE * f)
{
    unsigned char c1;
    if (!fread(&c1, 1, 1, f)) {
        fprintf(stderr,
                "\nError: fread return a number of element different from the expected.\n");
        return 0;
    }
    return c1;
}
