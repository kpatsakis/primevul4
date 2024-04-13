void zend_set_timeout(zend_long seconds, int reset_signals) /* {{{ */
{

	EG(timeout_seconds) = seconds;

#ifdef ZEND_WIN32
	if(!seconds) {
		return;
	}

        /* Don't use ChangeTimerQueueTimer() as it will not restart an expired
		timer, so we could end up with just an ignored timeout. Instead
		delete and recreate. */
	if (NULL != tq_timer) {
		if (!DeleteTimerQueueTimer(NULL, tq_timer, NULL)) {
			EG(timed_out) = 0;
			tq_timer = NULL;
			zend_error_noreturn(E_ERROR, "Could not delete queued timer");
			return;
		}
		tq_timer = NULL;
	}

	/* XXX passing NULL means the default timer queue provided by the system is used */
	if (!CreateTimerQueueTimer(&tq_timer, NULL, (WAITORTIMERCALLBACK)tq_timer_cb, (VOID*)&EG(timed_out), seconds*1000, 0, WT_EXECUTEONLYONCE)) {
		EG(timed_out) = 0;
		tq_timer = NULL;
		zend_error_noreturn(E_ERROR, "Could not queue new timer");
		return;
	}
	EG(timed_out) = 0;
#else
#	ifdef HAVE_SETITIMER
	{
		struct itimerval t_r;		/* timeout requested */
		int signo;

		if(seconds) {
			t_r.it_value.tv_sec = seconds;
			t_r.it_value.tv_usec = t_r.it_interval.tv_sec = t_r.it_interval.tv_usec = 0;

#	ifdef __CYGWIN__
			setitimer(ITIMER_REAL, &t_r, NULL);
		}
		signo = SIGALRM;
#	else
			setitimer(ITIMER_PROF, &t_r, NULL);
		}
		signo = SIGPROF;
#	endif

		if (reset_signals) {
#	ifdef ZEND_SIGNALS
			zend_signal(signo, zend_timeout);
#	else
			sigset_t sigset;

			signal(signo, zend_timeout);
			sigemptyset(&sigset);
			sigaddset(&sigset, signo);
			sigprocmask(SIG_UNBLOCK, &sigset, NULL);
#	endif
		}
	}
#	endif /* HAVE_SETITIMER */
#endif
}
/* }}} */

void zend_unset_timeout(void) /* {{{ */
{
#ifdef ZEND_WIN32
	if (NULL != tq_timer) {
		if (!DeleteTimerQueueTimer(NULL, tq_timer, NULL)) {
			EG(timed_out) = 0;
			tq_timer = NULL;
			zend_error_noreturn(E_ERROR, "Could not delete queued timer");
			return;
		}
		tq_timer = NULL;
	}
	EG(timed_out) = 0;
#else
#	ifdef HAVE_SETITIMER
	if (EG(timeout_seconds)) {
		struct itimerval no_timeout;

		no_timeout.it_value.tv_sec = no_timeout.it_value.tv_usec = no_timeout.it_interval.tv_sec = no_timeout.it_interval.tv_usec = 0;

#ifdef __CYGWIN__
		setitimer(ITIMER_REAL, &no_timeout, NULL);
#else
		setitimer(ITIMER_PROF, &no_timeout, NULL);
#endif
	}
#	endif
#endif
}
/* }}} */

zend_class_entry *zend_fetch_class(zend_string *class_name, int fetch_type) /* {{{ */
{
	zend_class_entry *ce;
	int fetch_sub_type = fetch_type & ZEND_FETCH_CLASS_MASK;

check_fetch_type:
	switch (fetch_sub_type) {
		case ZEND_FETCH_CLASS_SELF:
			if (UNEXPECTED(!EG(scope))) {
				zend_throw_or_error(fetch_type, NULL, "Cannot access self:: when no class scope is active");
			}
			return EG(scope);
		case ZEND_FETCH_CLASS_PARENT:
			if (UNEXPECTED(!EG(scope))) {
				zend_throw_or_error(fetch_type, NULL, "Cannot access parent:: when no class scope is active");
				return NULL;
			}
			if (UNEXPECTED(!EG(scope)->parent)) {
				zend_throw_or_error(fetch_type, NULL, "Cannot access parent:: when current class scope has no parent");
			}
			return EG(scope)->parent;
		case ZEND_FETCH_CLASS_STATIC:
			ce = zend_get_called_scope(EG(current_execute_data));
			if (UNEXPECTED(!ce)) {
				zend_throw_or_error(fetch_type, NULL, "Cannot access static:: when no class scope is active");
				return NULL;
			}
			return ce;
		case ZEND_FETCH_CLASS_AUTO: {
				fetch_sub_type = zend_get_class_fetch_type(class_name);
				if (UNEXPECTED(fetch_sub_type != ZEND_FETCH_CLASS_DEFAULT)) {
					goto check_fetch_type;
				}
			}
			break;
	}

	if (fetch_type & ZEND_FETCH_CLASS_NO_AUTOLOAD) {
		return zend_lookup_class_ex(class_name, NULL, 0);
	} else if ((ce = zend_lookup_class_ex(class_name, NULL, 1)) == NULL) {
		if (!(fetch_type & ZEND_FETCH_CLASS_SILENT) && !EG(exception)) {
			if (fetch_sub_type == ZEND_FETCH_CLASS_INTERFACE) {
				zend_throw_or_error(fetch_type, NULL, "Interface '%s' not found", ZSTR_VAL(class_name));
			} else if (fetch_sub_type == ZEND_FETCH_CLASS_TRAIT) {
				zend_throw_or_error(fetch_type, NULL, "Trait '%s' not found", ZSTR_VAL(class_name));
			} else {
				zend_throw_or_error(fetch_type, NULL, "Class '%s' not found", ZSTR_VAL(class_name));
			}
		}
		return NULL;
	}
	return ce;
}
/* }}} */

zend_class_entry *zend_fetch_class_by_name(zend_string *class_name, const zval *key, int fetch_type) /* {{{ */
{
	zend_class_entry *ce;

	if (fetch_type & ZEND_FETCH_CLASS_NO_AUTOLOAD) {
		return zend_lookup_class_ex(class_name, key, 0);
	} else if ((ce = zend_lookup_class_ex(class_name, key, 1)) == NULL) {
		if ((fetch_type & ZEND_FETCH_CLASS_SILENT) == 0 && !EG(exception)) {
			if ((fetch_type & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_INTERFACE) {
				zend_throw_or_error(fetch_type, NULL, "Interface '%s' not found", ZSTR_VAL(class_name));
			} else if ((fetch_type & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_TRAIT) {
				zend_throw_or_error(fetch_type, NULL, "Trait '%s' not found", ZSTR_VAL(class_name));
			} else {
				zend_throw_or_error(fetch_type, NULL, "Class '%s' not found", ZSTR_VAL(class_name));
			}
		}
		return NULL;
	}
	return ce;
}
/* }}} */

#define MAX_ABSTRACT_INFO_CNT 3
#define MAX_ABSTRACT_INFO_FMT "%s%s%s%s"
#define DISPLAY_ABSTRACT_FN(idx) \
	ai.afn[idx] ? ZEND_FN_SCOPE_NAME(ai.afn[idx]) : "", \
	ai.afn[idx] ? "::" : "", \
	ai.afn[idx] ? ZSTR_VAL(ai.afn[idx]->common.function_name) : "", \
	ai.afn[idx] && ai.afn[idx + 1] ? ", " : (ai.afn[idx] && ai.cnt > MAX_ABSTRACT_INFO_CNT ? ", ..." : "")

typedef struct _zend_abstract_info {
