void killchild(gpointer key, gpointer value, gpointer user_data) {
	pid_t *pid=value;

	kill(*pid, SIGTERM);
}
