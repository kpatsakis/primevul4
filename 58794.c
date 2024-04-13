static struct passwd *passwd_dup(pool *p, struct passwd *pw) {
  struct passwd *npw;

  npw = pcalloc(p, sizeof(struct passwd));

  npw->pw_name = pstrdup(p, pw->pw_name);
  npw->pw_passwd = pstrdup(p, pw->pw_passwd);
  npw->pw_uid = pw->pw_uid;
  npw->pw_gid = pw->pw_gid;
  npw->pw_gecos = pstrdup(p, pw->pw_gecos);
  npw->pw_dir = pstrdup(p, pw->pw_dir);
  npw->pw_shell = pstrdup(p, pw->pw_shell);

  return npw;
}
