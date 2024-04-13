void module_register(void) {
  plugin_register_complex_config("snmp", csnmp_config);
  plugin_register_init("snmp", csnmp_init);
  plugin_register_shutdown("snmp", csnmp_shutdown);
} /* void module_register */
