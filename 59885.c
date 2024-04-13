rend_service_check_private_dir_impl(const or_options_t *options,
                                    const rend_service_t *s,
                                    int create)
{
  cpd_check_t  check_opts = CPD_NONE;
  if (create) {
    check_opts |= CPD_CREATE;
  } else {
    check_opts |= CPD_CHECK_MODE_ONLY;
    check_opts |= CPD_CHECK;
  }
  if (s->dir_group_readable) {
    check_opts |= CPD_GROUP_READ;
  }
  /* Check/create directory */
  if (check_private_dir(s->directory, check_opts, options->User) < 0) {
    log_warn(LD_REND, "Checking service directory %s failed.", s->directory);
    return -1;
  }

  return 0;
}
