static void print_usage(int argc, char *argv[])
{
    char *name = NULL;
    name = strrchr(argv[0], '/');
    printf("Usage: %s -i|--infile FILE [-o|--outfile FILE] [-d|--debug]\n", (name ? name + 1: argv[0]));
    printf("Convert a plist FILE from binary to XML format or vice-versa.\n\n");
    printf("  -i, --infile FILE\tThe FILE to convert from\n");
    printf("  -o, --outfile FILE\tOptional FILE to convert to or stdout if not used\n");
    printf("  -d, --debug\t\tEnable extended debug output\n");
    printf("\n");
}
