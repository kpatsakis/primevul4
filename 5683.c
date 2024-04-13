static void print_usage(void)
{
	printf(
	"Usage:\n"
	"  booth list [options]\n"
	"  booth {grant|revoke} [options] <ticket>\n"
	"  booth status [options]\n"
	"\n"
	"  list:	     List all tickets\n"
	"  grant:        Grant ticket to site\n"
	"  revoke:       Revoke ticket\n"
	"\n"
	"Options:\n"
	"  -c FILE       Specify config file [default " BOOTH_DEFAULT_CONF "]\n"
	"                Can be a path or just a name without \".conf\" suffix\n"
	"  -s <site>     Connect/grant to a different site\n"
	"  -F            Try to grant the ticket immediately\n"
	"                even if not all sites are reachable\n"
	"                For manual tickets:\n"
	"                grant a manual ticket even if it has been already granted\n"
	"  -w            Wait forever for the outcome of the request\n"
	"  -C            Wait until the ticket is committed to the CIB (grant only)\n"
	"  -h            Print this help\n"
	"\n"
	"Examples:\n"
	"\n"
	"  # booth list (list tickets)\n"
	"  # booth grant ticket-A (grant ticket here)\n"
	"  # booth grant -s 10.121.8.183 ticket-A (grant ticket to site 10.121.8.183)\n"
	"  # booth revoke ticket-A (revoke ticket)\n"
	"\n"
	"See the booth(8) man page for more details.\n"
	);
}