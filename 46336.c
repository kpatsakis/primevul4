mm_answer_audit_command(int socket, Buffer *m)
{
	u_int len;
	char *cmd;

	debug3("%s entering", __func__);
	cmd = buffer_get_string(m, &len);
	/* sanity check command, if so how? */
	audit_run_command(cmd);
	free(cmd);
	return (0);
}
