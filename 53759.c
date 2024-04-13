ZEND_API zend_class_entry *zend_lookup_class_ex(zend_string *name, const zval *key, int use_autoload) /* {{{ */
{
	zend_class_entry *ce = NULL;
	zval args[1];
	zval local_retval;
	int retval;
	zend_string *lc_name;
	zend_fcall_info fcall_info;
	zend_fcall_info_cache fcall_cache;

	if (key) {
		lc_name = Z_STR_P(key);
	} else {
		if (name == NULL || !ZSTR_LEN(name)) {
			return NULL;
		}

		if (ZSTR_VAL(name)[0] == '\\') {
			lc_name = zend_string_alloc(ZSTR_LEN(name) - 1, 0);
			zend_str_tolower_copy(ZSTR_VAL(lc_name), ZSTR_VAL(name) + 1, ZSTR_LEN(name) - 1);
		} else {
			lc_name = zend_string_tolower(name);
		}
	}

	ce = zend_hash_find_ptr(EG(class_table), lc_name);
	if (ce) {
		if (!key) {
			zend_string_release(lc_name);
		}
		return ce;
	}

	/* The compiler is not-reentrant. Make sure we __autoload() only during run-time
	 * (doesn't impact functionality of __autoload()
	*/
	if (!use_autoload || zend_is_compiling()) {
		if (!key) {
			zend_string_release(lc_name);
		}
		return NULL;
	}

	if (!EG(autoload_func)) {
		zend_function *func = zend_hash_str_find_ptr(EG(function_table), ZEND_AUTOLOAD_FUNC_NAME, sizeof(ZEND_AUTOLOAD_FUNC_NAME) - 1);
		if (func) {
			EG(autoload_func) = func;
		} else {
			if (!key) {
				zend_string_release(lc_name);
			}
			return NULL;
		}

	}

	/* Verify class name before passing it to __autoload() */
	if (strspn(ZSTR_VAL(name), "0123456789_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ\177\200\201\202\203\204\205\206\207\210\211\212\213\214\215\216\217\220\221\222\223\224\225\226\227\230\231\232\233\234\235\236\237\240\241\242\243\244\245\246\247\250\251\252\253\254\255\256\257\260\261\262\263\264\265\266\267\270\271\272\273\274\275\276\277\300\301\302\303\304\305\306\307\310\311\312\313\314\315\316\317\320\321\322\323\324\325\326\327\330\331\332\333\334\335\336\337\340\341\342\343\344\345\346\347\350\351\352\353\354\355\356\357\360\361\362\363\364\365\366\367\370\371\372\373\374\375\376\377\\") != ZSTR_LEN(name)) {
		if (!key) {
			zend_string_release(lc_name);
		}
		return NULL;
	}

	if (EG(in_autoload) == NULL) {
		ALLOC_HASHTABLE(EG(in_autoload));
		zend_hash_init(EG(in_autoload), 8, NULL, NULL, 0);
	}

	if (zend_hash_add_empty_element(EG(in_autoload), lc_name) == NULL) {
		if (!key) {
			zend_string_release(lc_name);
		}
		return NULL;
	}

	ZVAL_UNDEF(&local_retval);

	if (ZSTR_VAL(name)[0] == '\\') {
		ZVAL_STRINGL(&args[0], ZSTR_VAL(name) + 1, ZSTR_LEN(name) - 1);
	} else {
		ZVAL_STR_COPY(&args[0], name);
	}

	fcall_info.size = sizeof(fcall_info);
	fcall_info.function_table = EG(function_table);
	ZVAL_STR_COPY(&fcall_info.function_name, EG(autoload_func)->common.function_name);
	fcall_info.symbol_table = NULL;
	fcall_info.retval = &local_retval;
	fcall_info.param_count = 1;
	fcall_info.params = args;
	fcall_info.object = NULL;
	fcall_info.no_separation = 1;

	fcall_cache.initialized = 1;
	fcall_cache.function_handler = EG(autoload_func);
	fcall_cache.calling_scope = NULL;
	fcall_cache.called_scope = NULL;
	fcall_cache.object = NULL;

	zend_exception_save();
	retval = zend_call_function(&fcall_info, &fcall_cache);
	zend_exception_restore();

	zval_ptr_dtor(&args[0]);
	zval_dtor(&fcall_info.function_name);

	zend_hash_del(EG(in_autoload), lc_name);

	zval_ptr_dtor(&local_retval);

	if (retval == SUCCESS) {
		ce = zend_hash_find_ptr(EG(class_table), lc_name);
	}
	if (!key) {
		zend_string_release(lc_name);
	}
	return ce;
}
/* }}} */
