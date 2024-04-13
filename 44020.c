setmodir(argv)
    char **argv;
{
    if(*argv == NULL)
	return 0;
    if(!strcmp(*argv,"in")) {
        maxoctets_dir = PPP_OCTETS_DIRECTION_IN;
    } else if (!strcmp(*argv,"out")) {
        maxoctets_dir = PPP_OCTETS_DIRECTION_OUT;
    } else if (!strcmp(*argv,"max")) {
        maxoctets_dir = PPP_OCTETS_DIRECTION_MAXOVERAL;
    } else {
        maxoctets_dir = PPP_OCTETS_DIRECTION_SUM;
    }
    return 1;
}
