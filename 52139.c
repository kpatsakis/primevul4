iperf_parse_arguments(struct iperf_test *test, int argc, char **argv)
{
    static struct option longopts[] =
    {
        {"port", required_argument, NULL, 'p'},
        {"format", required_argument, NULL, 'f'},
        {"interval", required_argument, NULL, 'i'},
        {"daemon", no_argument, NULL, 'D'},
        {"one-off", no_argument, NULL, '1'},
        {"verbose", no_argument, NULL, 'V'},
        {"json", no_argument, NULL, 'J'},
        {"version", no_argument, NULL, 'v'},
        {"server", no_argument, NULL, 's'},
        {"client", required_argument, NULL, 'c'},
        {"udp", no_argument, NULL, 'u'},
        {"bandwidth", required_argument, NULL, 'b'},
        {"time", required_argument, NULL, 't'},
        {"bytes", required_argument, NULL, 'n'},
        {"blockcount", required_argument, NULL, 'k'},
        {"length", required_argument, NULL, 'l'},
        {"parallel", required_argument, NULL, 'P'},
        {"reverse", no_argument, NULL, 'R'},
        {"window", required_argument, NULL, 'w'},
        {"bind", required_argument, NULL, 'B'},
        {"set-mss", required_argument, NULL, 'M'},
        {"no-delay", no_argument, NULL, 'N'},
        {"version4", no_argument, NULL, '4'},
        {"version6", no_argument, NULL, '6'},
        {"tos", required_argument, NULL, 'S'},
        {"flowlabel", required_argument, NULL, 'L'},
        {"zerocopy", no_argument, NULL, 'Z'},
        {"omit", required_argument, NULL, 'O'},
        {"file", required_argument, NULL, 'F'},
        {"affinity", required_argument, NULL, 'A'},
        {"title", required_argument, NULL, 'T'},
#if defined(linux) && defined(TCP_CONGESTION)
        {"linux-congestion", required_argument, NULL, 'C'},
#endif
	{"get-server-output", no_argument, NULL, OPT_GET_SERVER_OUTPUT},
        {"debug", no_argument, NULL, 'd'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };
    int flag;
    int blksize;
    int server_flag, client_flag, rate_flag, duration_flag;
    char* comma;
    char* slash;

    blksize = 0;
    server_flag = client_flag = rate_flag = duration_flag = 0;
    while ((flag = getopt_long(argc, argv, "p:f:i:D1VJvsc:ub:t:n:k:l:P:Rw:B:M:N46S:L:ZO:F:A:T:C:dh", longopts, NULL)) != -1) {
        switch (flag) {
            case 'p':
                test->server_port = atoi(optarg);
                break;
            case 'f':
                test->settings->unit_format = *optarg;
                break;
            case 'i':
                /* XXX: could potentially want separate stat collection and reporting intervals,
                   but just set them to be the same for now */
                test->stats_interval = test->reporter_interval = atof(optarg);
                if ((test->stats_interval < MIN_INTERVAL || test->stats_interval > MAX_INTERVAL) && test->stats_interval != 0) {
                    i_errno = IEINTERVAL;
                    return -1;
                }
                break;
            case 'D':
		test->daemon = 1;
		server_flag = 1;
	        break;
            case '1':
		test->one_off = 1;
		server_flag = 1;
	        break;
            case 'V':
                test->verbose = 1;
                break;
            case 'J':
                test->json_output = 1;
                break;
            case 'v':
                printf("%s\n", version);
		system("uname -a");
                exit(0);
            case 's':
                if (test->role == 'c') {
                    i_errno = IESERVCLIENT;
                    return -1;
                }
		iperf_set_test_role(test, 's');
                break;
            case 'c':
                if (test->role == 's') {
                    i_errno = IESERVCLIENT;
                    return -1;
                }
		iperf_set_test_role(test, 'c');
		iperf_set_test_server_hostname(test, optarg);
                break;
            case 'u':
                set_protocol(test, Pudp);
		client_flag = 1;
                break;
            case 'b':
		slash = strchr(optarg, '/');
		if (slash) {
		    *slash = '\0';
		    ++slash;
		    test->settings->burst = atoi(slash);
		    if (test->settings->burst <= 0 ||
		        test->settings->burst > MAX_BURST) {
			i_errno = IEBURST;
			return -1;
		    }
		}
                test->settings->rate = unit_atof_rate(optarg);
		rate_flag = 1;
		client_flag = 1;
                break;
            case 't':
                test->duration = atoi(optarg);
                if (test->duration > MAX_TIME) {
                    i_errno = IEDURATION;
                    return -1;
                }
		duration_flag = 1;
		client_flag = 1;
                break;
            case 'n':
                test->settings->bytes = unit_atoi(optarg);
		client_flag = 1;
                break;
            case 'k':
                test->settings->blocks = unit_atoi(optarg);
		client_flag = 1;
                break;
            case 'l':
                blksize = unit_atoi(optarg);
		client_flag = 1;
                break;
            case 'P':
                test->num_streams = atoi(optarg);
                if (test->num_streams > MAX_STREAMS) {
                    i_errno = IENUMSTREAMS;
                    return -1;
                }
		client_flag = 1;
                break;
            case 'R':
		iperf_set_test_reverse(test, 1);
		client_flag = 1;
                break;
            case 'w':
                test->settings->socket_bufsize = unit_atof(optarg);
                if (test->settings->socket_bufsize > MAX_TCP_BUFFER) {
                    i_errno = IEBUFSIZE;
                    return -1;
                }
		client_flag = 1;
                break;
            case 'B':
                test->bind_address = strdup(optarg);
                break;
            case 'M':
                test->settings->mss = atoi(optarg);
                if (test->settings->mss > MAX_MSS) {
                    i_errno = IEMSS;
                    return -1;
                }
		client_flag = 1;
                break;
            case 'N':
                test->no_delay = 1;
		client_flag = 1;
                break;
            case '4':
                test->settings->domain = AF_INET;
                break;
            case '6':
                test->settings->domain = AF_INET6;
                break;
            case 'S':
                test->settings->tos = strtol(optarg, NULL, 0);
		client_flag = 1;
                break;
            case 'L':
#if defined(linux)
                test->settings->flowlabel = strtol(optarg, NULL, 0);
		if (test->settings->flowlabel < 1 || test->settings->flowlabel > 0xfffff) {
                    i_errno = IESETFLOW;
                    return -1;
		}
		client_flag = 1;
#else /* linux */
                i_errno = IEUNIMP;
                return -1;
#endif /* linux */
                break;
            case 'Z':
                if (!has_sendfile()) {
                    i_errno = IENOSENDFILE;
                    return -1;
                }
                test->zerocopy = 1;
		client_flag = 1;
                break;
            case 'O':
                test->omit = atoi(optarg);
                if (test->omit < 0 || test->omit > 60) {
                    i_errno = IEOMIT;
                    return -1;
                }
		client_flag = 1;
                break;
            case 'F':
                test->diskfile_name = optarg;
                break;
            case 'A':
                test->affinity = atoi(optarg);
                if (test->affinity < 0 || test->affinity > 1024) {
                    i_errno = IEAFFINITY;
                    return -1;
                }
		comma = strchr(optarg, ',');
		if (comma != NULL) {
		    test->server_affinity = atoi(comma+1);
		    if (test->server_affinity < 0 || test->server_affinity > 1024) {
			i_errno = IEAFFINITY;
			return -1;
		    }
		    client_flag = 1;
		}
                break;
            case 'T':
                test->title = strdup(optarg);
		client_flag = 1;
                break;
	    case 'C':
#if defined(linux) && defined(TCP_CONGESTION)
		test->congestion = strdup(optarg);
		client_flag = 1;
#else /* linux */
		i_errno = IEUNIMP;
		return -1;
#endif /* linux */
		break;
	    case 'd':
		test->debug = 1;
		break;
	    case OPT_GET_SERVER_OUTPUT:
		test->get_server_output = 1;
		client_flag = 1;
		break;
            case 'h':
            default:
                usage_long();
                exit(1);
        }
    }

    /* Check flag / role compatibility. */
    if (test->role == 'c' && server_flag) {
	i_errno = IESERVERONLY;
	return -1;
    }
    if (test->role == 's' && client_flag) {
	i_errno = IECLIENTONLY;
	return -1;
    }

    if (blksize == 0) {
	if (test->protocol->id == Pudp)
	    blksize = DEFAULT_UDP_BLKSIZE;
	else
	    blksize = DEFAULT_TCP_BLKSIZE;
    }
    if (blksize <= 0 || blksize > MAX_BLOCKSIZE) {
	i_errno = IEBLOCKSIZE;
	return -1;
    }
    if (test->protocol->id == Pudp &&
	blksize > MAX_UDP_BLOCKSIZE) {
	i_errno = IEUDPBLOCKSIZE;
	return -1;
    }
    test->settings->blksize = blksize;

    if (!rate_flag)
	test->settings->rate = test->protocol->id == Pudp ? UDP_RATE : 0;

    if ((test->settings->bytes != 0 || test->settings->blocks != 0) && ! duration_flag)
        test->duration = 0;

    /* Disallow specifying multiple test end conditions. The code actually
    ** works just fine without this prohibition. As soon as any one of the
    ** three possible end conditions is met, the test ends. So this check
    ** could be removed if desired.
    */
    if ((duration_flag && test->settings->bytes != 0) ||
        (duration_flag && test->settings->blocks != 0) ||
	(test->settings->bytes != 0 && test->settings->blocks != 0)) {
        i_errno = IEENDCONDITIONS;
        return -1;
    }

    /* For subsequent calls to getopt */
#ifdef __APPLE__
    optreset = 1;
#endif
    optind = 0;

    if ((test->role != 'c') && (test->role != 's')) {
        i_errno = IENOROLE;
        return -1;
    }

    return 0;
}
