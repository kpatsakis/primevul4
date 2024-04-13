local int option(char *arg)
{
    static int get = 0;     /* if not zero, look for option parameter */
    char bad[3] = "-X";     /* for error messages (X is replaced) */

    /* if no argument or dash option, check status of get */
    if (get && (arg == NULL || *arg == '-')) {
        bad[1] = "bpSIM"[get - 1];
        bail("missing parameter after ", bad);
    }
    if (arg == NULL)
        return 0;

    /* process long option or short options */
    if (*arg == '-') {
        /* a single dash will be interpreted as stdin */
        if (*++arg == 0)
            return 1;

        /* process long option (fall through with equivalent short option) */
        if (*arg == '-') {
            int j;

            arg++;
            for (j = NLOPTS - 1; j >= 0; j--)
                if (strcmp(arg, longopts[j][0]) == 0) {
                    arg = longopts[j][1];
                    break;
                }
            if (j < 0)
                bail("invalid option: ", arg - 2);
        }

        /* process short options (more than one allowed after dash) */
        do {
            /* if looking for a parameter, don't process more single character
               options until we have the parameter */
            if (get) {
                if (get == 3)
                    bail("invalid usage: -s must be followed by space", "");
                break;      /* allow -pnnn and -bnnn, fall to parameter code */
            }

            /* process next single character option or compression level */
            bad[1] = *arg;
            switch (*arg) {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                g.level = *arg - '0';
                while (arg[1] >= '0' && arg[1] <= '9') {
                    if (g.level && (INT_MAX - (arg[1] - '0')) / g.level < 10)
                        bail("only levels 0..9 and 11 are allowed", "");
                    g.level = g.level * 10 + *++arg - '0';
                }
                if (g.level == 10 || g.level > 11)
                    bail("only levels 0..9 and 11 are allowed", "");
                new_opts();
                break;
            case 'F':  g.zopts.blocksplittinglast = 1;  break;
            case 'I':  get = 4;  break;
            case 'K':  g.form = 2;  g.sufx = ".zip";  break;
            case 'L':
                fputs(VERSION, stderr);
                fputs("Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012, 2013"
                      " Mark Adler\n",
                      stderr);
                fputs("Subject to the terms of the zlib license.\n",
                      stderr);
                fputs("No warranty is provided or implied.\n", stderr);
                exit(0);
            case 'M':  get = 5;  break;
            case 'N':  g.headis |= 0xf;  break;
            case 'O':  g.zopts.blocksplitting = 0;  break;
            case 'R':  g.rsync = 1;  break;
            case 'S':  get = 3;  break;
            case 'T':  g.headis &= ~0xa;  break;
            case 'V':  fputs(VERSION, stderr);  exit(0);
            case 'Z':
                bail("invalid option: LZW output not supported: ", bad);
            case 'a':
                bail("invalid option: ascii conversion not supported: ", bad);
            case 'b':  get = 1;  break;
            case 'c':  g.pipeout = 1;  break;
            case 'd':  if (!g.decode) g.headis >>= 2;  g.decode = 1;  break;
            case 'f':  g.force = 1;  break;
            case 'h':  help();  break;
            case 'i':  g.setdict = 0;  break;
            case 'k':  g.keep = 1;  break;
            case 'l':  g.list = 1;  break;
            case 'n':  g.headis &= ~5;  break;
            case 'p':  get = 2;  break;
            case 'q':  g.verbosity = 0;  break;
            case 'r':  g.recurse = 1;  break;
            case 't':  g.decode = 2;  break;
            case 'v':  g.verbosity++;  break;
            case 'z':  g.form = 1;  g.sufx = ".zz";  break;
            default:
                bail("invalid option: ", bad);
            }
        } while (*++arg);
        if (*arg == 0)
            return 0;
    }

    /* process option parameter for -b, -p, -S, -I, or -M */
    if (get) {
        size_t n;

        if (get == 1) {
            n = num(arg);
            g.block = n << 10;                  /* chunk size */
            if (g.block < DICT)
                bail("block size too small (must be >= 32K)", "");
            if (n != g.block >> 10 ||
                OUTPOOL(g.block) < g.block ||
                (ssize_t)OUTPOOL(g.block) < 0 ||
                g.block > (1UL << 29))          /* limited by append_len() */
                bail("block size too large: ", arg);
            new_opts();
        }
        else if (get == 2) {
            n = num(arg);
            g.procs = (int)n;                   /* # processes */
            if (g.procs < 1)
                bail("invalid number of processes: ", arg);
            if ((size_t)g.procs != n || INBUFS(g.procs) < 1)
                bail("too many processes: ", arg);
#ifdef NOTHREAD
            if (g.procs > 1)
                bail("compiled without threads", "");
#endif
            new_opts();
        }
        else if (get == 3)
            g.sufx = arg;                       /* gz suffix */
        else if (get == 4)
            g.zopts.numiterations = num(arg);   /* optimization iterations */
        else if (get == 5)
            g.zopts.blocksplittingmax = num(arg);   /* max block splits */
        get = 0;
        return 0;
    }

    /* neither an option nor parameter */
    return 1;
}
