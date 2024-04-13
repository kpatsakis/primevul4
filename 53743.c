ZEND_API int zend_delete_global_variable(zend_string *name) /* {{{ */
{
    return zend_hash_del_ind(&EG(symbol_table), name);
}
/* }}} */
