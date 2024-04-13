static int csnmp_config_add_data(oconfig_item_t *ci) {
  data_definition_t *dd;
  int status = 0;

  dd = calloc(1, sizeof(*dd));
  if (dd == NULL)
    return (-1);

  status = cf_util_get_string(ci, &dd->name);
  if (status != 0) {
    free(dd);
    return (-1);
  }

  dd->scale = 1.0;
  dd->shift = 0.0;

  for (int i = 0; i < ci->children_num; i++) {
    oconfig_item_t *option = ci->children + i;

    if (strcasecmp("Type", option->key) == 0)
      status = cf_util_get_string(option, &dd->type);
    else if (strcasecmp("Table", option->key) == 0)
      status = cf_util_get_boolean(option, &dd->is_table);
    else if (strcasecmp("Instance", option->key) == 0)
      status = csnmp_config_add_data_instance(dd, option);
    else if (strcasecmp("InstancePrefix", option->key) == 0)
      status = csnmp_config_add_data_instance_prefix(dd, option);
    else if (strcasecmp("Values", option->key) == 0)
      status = csnmp_config_add_data_values(dd, option);
    else if (strcasecmp("Shift", option->key) == 0)
      status = cf_util_get_double(option, &dd->shift);
    else if (strcasecmp("Scale", option->key) == 0)
      status = cf_util_get_double(option, &dd->scale);
    else if (strcasecmp("Ignore", option->key) == 0)
      status = csnmp_config_add_data_blacklist(dd, option);
    else if (strcasecmp("InvertMatch", option->key) == 0)
      status = csnmp_config_add_data_blacklist_match_inverted(dd, option);
    else {
      WARNING("snmp plugin: Option `%s' not allowed here.", option->key);
      status = -1;
    }

    if (status != 0)
      break;
  } /* for (ci->children) */

  while (status == 0) {
    if (dd->type == NULL) {
      WARNING("snmp plugin: `Type' not given for data `%s'", dd->name);
      status = -1;
      break;
    }
    if (dd->values == NULL) {
      WARNING("snmp plugin: No `Value' given for data `%s'", dd->name);
      status = -1;
      break;
    }

    break;
  } /* while (status == 0) */

  if (status != 0) {
    sfree(dd->name);
    sfree(dd->instance_prefix);
    sfree(dd->values);
    sfree(dd->ignores);
    sfree(dd);
    return (-1);
  }

  DEBUG("snmp plugin: dd = { name = %s, type = %s, is_table = %s, values_len = "
        "%zu }",
        dd->name, dd->type, (dd->is_table != 0) ? "true" : "false",
        dd->values_len);

  if (data_head == NULL)
    data_head = dd;
  else {
    data_definition_t *last;
    last = data_head;
    while (last->next != NULL)
      last = last->next;
    last->next = dd;
  }

  return (0);
} /* int csnmp_config_add_data */
