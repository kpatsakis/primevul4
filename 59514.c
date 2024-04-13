static options_t *parse_arguments(int argc, char *argv[])
{
    int i = 0;

    options_t *options = (options_t *) malloc(sizeof(options_t));
    memset(options, 0, sizeof(options_t));

    for (i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "--infile") || !strcmp(argv[i], "-i"))
        {
            if ((i + 1) == argc)
            {
                free(options);
                return NULL;
            }
            options->in_file = argv[i + 1];
            i++;
            continue;
        }

        if (!strcmp(argv[i], "--outfile") || !strcmp(argv[i], "-o"))
        {
            if ((i + 1) == argc)
            {
                free(options);
                return NULL;
            }
            options->out_file = argv[i + 1];
            i++;
            continue;
        }

        if (!strcmp(argv[i], "--debug") || !strcmp(argv[i], "-d"))
        {
            options->debug = 1;
        }

        if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h"))
        {
            free(options);
            return NULL;
        }
    }

    if (!options->in_file)
    {
        free(options);
        return NULL;
    }

    return options;
}
