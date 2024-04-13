get_gdb_index_symbol_kind_name (gdb_index_symbol_kind kind)
{
  /* See gdb/gdb-index.h.  */
  static const char * const kinds[] =
  {
    N_ ("no info"),
    N_ ("type"),
    N_ ("variable"),
    N_ ("function"),
    N_ ("other"),
    N_ ("unused5"),
    N_ ("unused6"),
    N_ ("unused7")
  };

  return _ (kinds[kind]);
}