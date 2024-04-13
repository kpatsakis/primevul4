static void check_and_drop(void *_data)
{
	struct detach_data *data = _data;

	if (!data->mountpoint && list_empty(&data->select.dispose))
		__d_drop(data->select.start);
}
