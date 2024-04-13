static void my_handler(int s){
	EUID_ROOT();
	if (!arg_quiet) {
		printf("\nParent received signal %d, shutting down the child process...\n", s);
		fflush(0);
	}
	logsignal(s);
	kill(child, SIGTERM);
	myexit(1);
}
