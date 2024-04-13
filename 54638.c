static int csnmp_instance_list_add(csnmp_list_instances_t **head,
                                   csnmp_list_instances_t **tail,
                                   const struct snmp_pdu *res,
                                   const host_definition_t *hd,
                                   const data_definition_t *dd) {
  csnmp_list_instances_t *il;
  struct variable_list *vb;
  oid_t vb_name;
  int status;
  uint32_t is_matched;

  /* Set vb on the last variable */
  for (vb = res->variables; (vb != NULL) && (vb->next_variable != NULL);
       vb = vb->next_variable)
    /* do nothing */;
  if (vb == NULL)
    return (-1);

  csnmp_oid_init(&vb_name, vb->name, vb->name_length);

  il = calloc(1, sizeof(*il));
  if (il == NULL) {
    ERROR("snmp plugin: calloc failed.");
    return (-1);
  }
  il->next = NULL;

  status = csnmp_oid_suffix(&il->suffix, &vb_name, &dd->instance.oid);
  if (status != 0) {
    sfree(il);
    return (status);
  }

  /* Get instance name */
  if ((vb->type == ASN_OCTET_STR) || (vb->type == ASN_BIT_STR) ||
      (vb->type == ASN_IPADDRESS)) {
    char *ptr;

    csnmp_strvbcopy(il->instance, vb, sizeof(il->instance));
    is_matched = 0;
    for (uint32_t i = 0; i < dd->ignores_len; i++) {
      status = fnmatch(dd->ignores[i], il->instance, 0);
      if (status == 0) {
        if (dd->invert_match == 0) {
          sfree(il);
          return 0;
        } else {
          is_matched = 1;
          break;
        }
      }
    }
    if (dd->invert_match != 0 && is_matched == 0) {
      sfree(il);
      return 0;
    }
    for (ptr = il->instance; *ptr != '\0'; ptr++) {
      if ((*ptr > 0) && (*ptr < 32))
        *ptr = ' ';
      else if (*ptr == '/')
        *ptr = '_';
    }
    DEBUG("snmp plugin: il->instance = `%s';", il->instance);
  } else {
    value_t val = csnmp_value_list_to_value(
        vb, DS_TYPE_COUNTER,
        /* scale = */ 1.0, /* shift = */ 0.0, hd->name, dd->name);
    ssnprintf(il->instance, sizeof(il->instance), "%llu", val.counter);
  }

  /* TODO: Debugging output */

  if (*head == NULL)
    *head = il;
  else
    (*tail)->next = il;
  *tail = il;

  return (0);
} /* int csnmp_instance_list_add */
