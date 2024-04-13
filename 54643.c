static int csnmp_read_host(user_data_t *ud) {
  host_definition_t *host;
  int status;
  int success;
  int i;

  host = ud->data;

  if (host->interval == 0)
    host->interval = plugin_get_interval();

  if (host->sess_handle == NULL)
    csnmp_host_open_session(host);

  if (host->sess_handle == NULL)
    return (-1);

  success = 0;
  for (i = 0; i < host->data_list_len; i++) {
    data_definition_t *data = host->data_list[i];

    if (data->is_table)
      status = csnmp_read_table(host, data);
    else
      status = csnmp_read_value(host, data);

    if (status == 0)
      success++;
  }

  if (success == 0)
    return (-1);

  return (0);
} /* int csnmp_read_host */
