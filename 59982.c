get_n_primary_guards(void)
{
  const int n = get_options()->NumEntryGuards;
  const int n_dir = get_options()->NumDirectoryGuards;
  if (n > 5) {
    return MAX(n_dir, n + n / 2);
  } else if (n >= 1) {
    return MAX(n_dir, n * 2);
  }

  return networkstatus_get_param(NULL,
                                 "guard-n-primary-guards",
                                 DFLT_N_PRIMARY_GUARDS, 1, INT32_MAX);
}
