static int csnmp_dispatch_table(host_definition_t *host,
                                data_definition_t *data,
                                csnmp_list_instances_t *instance_list,
                                csnmp_table_values_t **value_table) {
  const data_set_t *ds;
  value_list_t vl = VALUE_LIST_INIT;

  csnmp_list_instances_t *instance_list_ptr;
  csnmp_table_values_t **value_table_ptr;

  size_t i;
  _Bool have_more;
  oid_t current_suffix;

  ds = plugin_get_ds(data->type);
  if (!ds) {
    ERROR("snmp plugin: DataSet `%s' not defined.", data->type);
    return (-1);
  }
  assert(ds->ds_num == data->values_len);
  assert(data->values_len > 0);

  instance_list_ptr = instance_list;

  value_table_ptr = calloc(data->values_len, sizeof(*value_table_ptr));
  if (value_table_ptr == NULL)
    return (-1);
  for (i = 0; i < data->values_len; i++)
    value_table_ptr[i] = value_table[i];

  vl.values_len = data->values_len;
  vl.values = malloc(sizeof(*vl.values) * vl.values_len);
  if (vl.values == NULL) {
    ERROR("snmp plugin: malloc failed.");
    sfree(value_table_ptr);
    return (-1);
  }

  sstrncpy(vl.host, host->name, sizeof(vl.host));
  sstrncpy(vl.plugin, "snmp", sizeof(vl.plugin));

  vl.interval = host->interval;

  have_more = 1;
  while (have_more) {
    _Bool suffix_skipped = 0;

    /* Determine next suffix to handle. */
    if (instance_list != NULL) {
      if (instance_list_ptr == NULL) {
        have_more = 0;
        continue;
      }

      memcpy(&current_suffix, &instance_list_ptr->suffix,
             sizeof(current_suffix));
    } else /* no instance configured */
    {
      csnmp_table_values_t *ptr = value_table_ptr[0];
      if (ptr == NULL) {
        have_more = 0;
        continue;
      }

      memcpy(&current_suffix, &ptr->suffix, sizeof(current_suffix));
    }

    /* Update all the value_table_ptr to point at the entry with the same
     * trailing partial OID */
    for (i = 0; i < data->values_len; i++) {
      while (
          (value_table_ptr[i] != NULL) &&
          (csnmp_oid_compare(&value_table_ptr[i]->suffix, &current_suffix) < 0))
        value_table_ptr[i] = value_table_ptr[i]->next;

      if (value_table_ptr[i] == NULL) {
        have_more = 0;
        break;
      } else if (csnmp_oid_compare(&value_table_ptr[i]->suffix,
                                   &current_suffix) > 0) {
        /* This suffix is missing in the subtree. Indicate this with the
         * "suffix_skipped" flag and try the next instance / suffix. */
        suffix_skipped = 1;
        break;
      }
    } /* for (i = 0; i < columns; i++) */

    if (!have_more)
      break;

    /* Matching the values failed. Start from the beginning again. */
    if (suffix_skipped) {
      if (instance_list != NULL)
        instance_list_ptr = instance_list_ptr->next;
      else
        value_table_ptr[0] = value_table_ptr[0]->next;

      continue;
    }

/* if we reach this line, all value_table_ptr[i] are non-NULL and are set
 * to the same subid. instance_list_ptr is either NULL or points to the
 * same subid, too. */
#if COLLECT_DEBUG
    for (i = 1; i < data->values_len; i++) {
      assert(value_table_ptr[i] != NULL);
      assert(csnmp_oid_compare(&value_table_ptr[i - 1]->suffix,
                               &value_table_ptr[i]->suffix) == 0);
    }
    assert((instance_list_ptr == NULL) ||
           (csnmp_oid_compare(&instance_list_ptr->suffix,
                              &value_table_ptr[0]->suffix) == 0));
#endif

    sstrncpy(vl.type, data->type, sizeof(vl.type));

    {
      char temp[DATA_MAX_NAME_LEN];

      if (instance_list_ptr == NULL)
        csnmp_oid_to_string(temp, sizeof(temp), &current_suffix);
      else
        sstrncpy(temp, instance_list_ptr->instance, sizeof(temp));

      if (data->instance_prefix == NULL)
        sstrncpy(vl.type_instance, temp, sizeof(vl.type_instance));
      else
        ssnprintf(vl.type_instance, sizeof(vl.type_instance), "%s%s",
                  data->instance_prefix, temp);
    }

    for (i = 0; i < data->values_len; i++)
      vl.values[i] = value_table_ptr[i]->value;

    /* If we get here `vl.type_instance' and all `vl.values' have been set
     * vl.type_instance can be empty, i.e. a blank port description on a
     * switch if you're using IF-MIB::ifDescr as Instance.
     */
    if (vl.type_instance[0] != '\0')
      plugin_dispatch_values(&vl);

    if (instance_list != NULL)
      instance_list_ptr = instance_list_ptr->next;
    else
      value_table_ptr[0] = value_table_ptr[0]->next;
  } /* while (have_more) */

  sfree(vl.values);
  sfree(value_table_ptr);

  return (0);
} /* int csnmp_dispatch_table */
