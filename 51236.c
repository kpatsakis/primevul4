abort_sasl_exit(hook_data_client_exit *data)
{
	if (data->target->localClient)
		abort_sasl(data->target);
}
