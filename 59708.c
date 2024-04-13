void clear_run_files(pid_t pid) {
	bandwidth_del_run_file(pid);		// bandwidth file
	network_del_run_file(pid);		// network map file
	delete_name_file(pid);
	delete_x11_file(pid);
}
