perf_event_read_event(struct perf_event *event,
			struct task_struct *task)
{
	struct perf_output_handle handle;
	struct perf_sample_data sample;
	struct perf_read_event read_event = {
		.header = {
			.type = PERF_RECORD_READ,
			.misc = 0,
			.size = sizeof(read_event) + event->read_size,
		},
		.pid = perf_event_pid(event, task),
		.tid = perf_event_tid(event, task),
	};
	int ret;

	perf_event_header__init_id(&read_event.header, &sample, event);
	ret = perf_output_begin(&handle, event, read_event.header.size);
	if (ret)
		return;

	perf_output_put(&handle, read_event);
	perf_output_read(&handle, event);
	perf_event__output_id_sample(event, &handle, &sample);

	perf_output_end(&handle);
}
