get_n_primary_guards_to_use(guard_usage_t usage)
{
  int configured;
  const char *param_name;
  int param_default;
  if (usage == GUARD_USAGE_DIRGUARD) {
    configured = get_options()->NumDirectoryGuards;
    param_name = "guard-n-primary-dir-guards-to-use";
    param_default = DFLT_N_PRIMARY_DIR_GUARDS_TO_USE;
  } else {
    configured = get_options()->NumEntryGuards;
    param_name = "guard-n-primary-guards-to-use";
    param_default = DFLT_N_PRIMARY_GUARDS_TO_USE;
  }
  if (configured >= 1) {
    return configured;
  }
  return networkstatus_get_param(NULL,
                                 param_name, param_default, 1, INT32_MAX);
}
