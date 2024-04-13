static int csnmp_read_value(host_definition_t *host, data_definition_t *data) {
  struct snmp_pdu *req;
  struct snmp_pdu *res = NULL;
  struct variable_list *vb;

  const data_set_t *ds;
  value_list_t vl = VALUE_LIST_INIT;

  int status;
  size_t i;

  DEBUG("snmp plugin: csnmp_read_value (host = %s, data = %s)", host->name,
        data->name);

  if (host->sess_handle == NULL) {
    DEBUG("snmp plugin: csnmp_read_value: host->sess_handle == NULL");
    return (-1);
  }

  ds = plugin_get_ds(data->type);
  if (!ds) {
    ERROR("snmp plugin: DataSet `%s' not defined.", data->type);
    return (-1);
  }

  if (ds->ds_num != data->values_len) {
    ERROR("snmp plugin: DataSet `%s' requires %zu values, but config talks "
          "about %zu",
          data->type, ds->ds_num, data->values_len);
    return (-1);
  }

  vl.values_len = ds->ds_num;
  vl.values = malloc(sizeof(*vl.values) * vl.values_len);
  if (vl.values == NULL)
    return (-1);
  for (i = 0; i < vl.values_len; i++) {
    if (ds->ds[i].type == DS_TYPE_COUNTER)
      vl.values[i].counter = 0;
    else
      vl.values[i].gauge = NAN;
  }

  sstrncpy(vl.host, host->name, sizeof(vl.host));
  sstrncpy(vl.plugin, "snmp", sizeof(vl.plugin));
  sstrncpy(vl.type, data->type, sizeof(vl.type));
  sstrncpy(vl.type_instance, data->instance.string, sizeof(vl.type_instance));

  vl.interval = host->interval;

  req = snmp_pdu_create(SNMP_MSG_GET);
  if (req == NULL) {
    ERROR("snmp plugin: snmp_pdu_create failed.");
    sfree(vl.values);
    return (-1);
  }

  for (i = 0; i < data->values_len; i++)
    snmp_add_null_var(req, data->values[i].oid, data->values[i].oid_len);

  status = snmp_sess_synch_response(host->sess_handle, req, &res);

  if ((status != STAT_SUCCESS) || (res == NULL)) {
    char *errstr = NULL;

    snmp_sess_error(host->sess_handle, NULL, NULL, &errstr);
    ERROR("snmp plugin: host %s: snmp_sess_synch_response failed: %s",
          host->name, (errstr == NULL) ? "Unknown problem" : errstr);

    if (res != NULL)
      snmp_free_pdu(res);

    sfree(errstr);
    sfree(vl.values);
    csnmp_host_close_session(host);

    return (-1);
  }

  for (vb = res->variables; vb != NULL; vb = vb->next_variable) {
#if COLLECT_DEBUG
    char buffer[1024];
    snprint_variable(buffer, sizeof(buffer), vb->name, vb->name_length, vb);
    DEBUG("snmp plugin: Got this variable: %s", buffer);
#endif /* COLLECT_DEBUG */

    for (i = 0; i < data->values_len; i++)
      if (snmp_oid_compare(data->values[i].oid, data->values[i].oid_len,
                           vb->name, vb->name_length) == 0)
        vl.values[i] =
            csnmp_value_list_to_value(vb, ds->ds[i].type, data->scale,
                                      data->shift, host->name, data->name);
  } /* for (res->variables) */

  snmp_free_pdu(res);

  DEBUG("snmp plugin: -> plugin_dispatch_values (&vl);");
  plugin_dispatch_values(&vl);
  sfree(vl.values);

  return (0);
} /* int csnmp_read_value */
