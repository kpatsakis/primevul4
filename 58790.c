static void ensure_open_passwd(pool *p) {
  /* Make sure pass/group is open. */
  pr_auth_setpwent(p);
  pr_auth_setgrent(p);

  /* On some unices the following is necessary to ensure the files
   * are open (BSDI 3.1)
   */
  pr_auth_getpwent(p);
  pr_auth_getgrent(p);

  /* Per Debian bug report:
   *   https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=717235
   * we might want to do another set{pw,gr}ent(), to play better with
   * some NSS modules.
   */
  pr_auth_setpwent(p);
  pr_auth_setgrent(p);
}
