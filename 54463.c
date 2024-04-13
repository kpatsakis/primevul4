static void *tcmur_cmdproc_thread(void *arg)
{
	struct tcmu_device *dev = arg;
	struct tcmur_handler *rhandler = tcmu_get_runner_handler(dev);
	struct pollfd pfd;
	int ret;

	pthread_cleanup_push(cmdproc_thread_cleanup, dev);

	while (1) {
                int completed = 0;
		struct tcmulib_cmd *cmd;

		tcmulib_processing_start(dev);

		while ((cmd = tcmulib_get_next_command(dev)) != NULL) {
			if (tcmu_get_log_level() == TCMU_LOG_DEBUG_SCSI_CMD)
				tcmu_cdb_debug_info(cmd);

			if (tcmur_handler_is_passthrough_only(rhandler))
				ret = tcmur_cmd_passthrough_handler(dev, cmd);
			else
				ret = tcmur_generic_handle_cmd(dev, cmd);

			if (ret == TCMU_NOT_HANDLED)
				tcmu_warn("Command 0x%x not supported\n", cmd->cdb[0]);

			/*
			 * command (processing) completion is called in the following
			 * scenarios:
			 *   - handle_cmd: synchronous handlers
			 *   - generic_handle_cmd: non tcmur handler calls (see generic_cmd())
			 *			   and on errors when calling tcmur handler.
			 */
			if (ret != TCMU_ASYNC_HANDLED) {
				completed = 1;
				tcmur_command_complete(dev, cmd, ret);
			}
		}

		if (completed)
			tcmulib_processing_complete(dev);

		pfd.fd = tcmu_get_dev_fd(dev);
		pfd.events = POLLIN;
		pfd.revents = 0;

		poll(&pfd, 1, -1);

		if (pfd.revents != POLLIN) {
			tcmu_err("poll received unexpected revent: 0x%x\n", pfd.revents);
			break;
		}
	}

	tcmu_err("thread terminating, should never happen\n");

	pthread_cleanup_pop(1);

	return NULL;
}
