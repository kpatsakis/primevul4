static int csnmp_shutdown(void) {
  data_definition_t *data_this;
  data_definition_t *data_next;

  /* When we get here, the read threads have been stopped and all the
   * `host_definition_t' will be freed. */
  DEBUG("snmp plugin: Destroying all data definitions.");

  data_this = data_head;
  data_head = NULL;
  while (data_this != NULL) {
    data_next = data_this->next;

    sfree(data_this->name);
    sfree(data_this->type);
    sfree(data_this->values);
    sfree(data_this->ignores);
    sfree(data_this);

    data_this = data_next;
  }

  return (0);
} /* int csnmp_shutdown */
