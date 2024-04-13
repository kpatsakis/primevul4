static gboolean sighandler(gpointer user_data)
{
	tcmulib_cleanup_all_cmdproc_threads();
	tcmu_cancel_log_thread();
	tcmu_cancel_config_thread(tcmu_cfg);

	g_main_loop_quit((GMainLoop*)user_data);

	return G_SOURCE_CONTINUE;
}
