 */
PHP_MINFO_FUNCTION(wddx)
{
	php_info_print_table_start();
#if HAVE_PHP_SESSION && !defined(COMPILE_DL_SESSION)
	php_info_print_table_header(2, "WDDX Support", "enabled" );
	php_info_print_table_row(2, "WDDX Session Serializer", "enabled" );
#else
	php_info_print_table_row(2, "WDDX Support", "enabled" );
#endif
	php_info_print_table_end();