void sigchld_handler(int s) {
        int status;
	int* i;
	pid_t pid;

	while((pid=waitpid(-1, &status, WNOHANG)) > 0) {
		if(WIFEXITED(status)) {
			msg(LOG_INFO, "Child exited with %d", WEXITSTATUS(status));
		}
		i=g_hash_table_lookup(children, &pid);
		if(!i) {
			msg(LOG_INFO, "SIGCHLD received for an unknown child with PID %ld", (long)pid);
		} else {
			DEBUG("Removing %d from the list of children", pid);
			g_hash_table_remove(children, &pid);
		}
	}
}
