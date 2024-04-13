readfile(argv)
    char **argv;
{
    return options_from_file(*argv, 1, 1, privileged_option);
}
