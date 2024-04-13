void usage() {
	printf("This is nbd-server version " VERSION "\n");
	printf("Usage: [ip:|ip6@]port file_to_export [size][kKmM] [-l authorize_file] [-r] [-m] [-c] [-C configuration file] [-p PID file name] [-o section name] [-M max connections]\n"
	       "\t-r|--read-only\t\tread only\n"
	       "\t-m|--multi-file\t\tmultiple file\n"
	       "\t-c|--copy-on-write\tcopy on write\n"
	       "\t-C|--config-file\tspecify an alternate configuration file\n"
	       "\t-l|--authorize-file\tfile with list of hosts that are allowed to\n\t\t\t\tconnect.\n"
	       "\t-p|--pid-file\t\tspecify a filename to write our PID to\n"
	       "\t-o|--output-config\toutput a config file section for what you\n\t\t\t\tspecified on the command line, with the\n\t\t\t\tspecified section name\n"
	       "\t-M|--max-connections\tspecify the maximum number of opened connections\n\n"
	       "\tif port is set to 0, stdin is used (for running from inetd).\n"
	       "\tif file_to_export contains '%%s', it is substituted with the IP\n"
	       "\t\taddress of the machine trying to connect\n" 
	       "\tif ip is set, it contains the local IP address on which we're listening.\n\tif not, the server will listen on all local IP addresses\n");
	printf("Using configuration file %s\n", CFILE);
}
