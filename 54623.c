static void call_snmp_init_once(void) {
  static int have_init = 0;

  if (have_init == 0)
    init_snmp(PACKAGE_NAME);
  have_init = 1;
} /* void call_snmp_init_once */
