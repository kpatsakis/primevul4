MODRET set_allowchrootsymlinks(cmd_rec *cmd) {
  int allow_chroot_symlinks = -1;
  config_rec *c = NULL;

  CHECK_ARGS(cmd, 1);
  CHECK_CONF(cmd, CONF_ROOT|CONF_VIRTUAL|CONF_GLOBAL);

  allow_chroot_symlinks = get_boolean(cmd, 1);
  if (allow_chroot_symlinks == -1) {
    CONF_ERROR(cmd, "expected Boolean parameter");
  }

  c = add_config_param(cmd->argv[0], 1, NULL);
  c->argv[0] = pcalloc(c->pool, sizeof(int));
  *((int *) c->argv[0]) = allow_chroot_symlinks;

  return PR_HANDLED(cmd);
}
