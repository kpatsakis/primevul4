service_is_single_onion_poisoned(const rend_service_t *service)
{
  char *poison_fname = NULL;
  file_status_t fstatus;

  /* Passing a NULL service is a bug */
  if (BUG(!service)) {
    return 0;
  }

  if (rend_service_is_ephemeral(service)) {
    return 0;
  }

  poison_fname = rend_service_sos_poison_path(service);

  fstatus = file_status(poison_fname);
  tor_free(poison_fname);

  /* If this fname is occupied, the hidden service has been poisoned.
   * fstatus can be FN_ERROR if the service directory does not exist, in that
   * case, there is obviously no private key. */
  if (fstatus == FN_FILE || fstatus == FN_EMPTY) {
    return 1;
  }

  return 0;
}
