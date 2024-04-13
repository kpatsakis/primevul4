static int load_our_module(void)
{
	struct kmod_list *list = NULL, *itr;
	struct kmod_ctx *ctx;
	struct stat sb;
	struct utsname u;
	int ret;

	ctx = kmod_new(NULL, NULL);
	if (!ctx) {
		tcmu_err("kmod_new() failed: %m\n");
		return -1;
	}

	ret = kmod_module_new_from_lookup(ctx, "target_core_user", &list);
	if (ret < 0) {
		/* In some environments like containers, /lib/modules/`uname -r`
		 * will not exist, in such cases the load module job be taken
		 * care by admin, either by manual load or makesure it's builtin
		 */
		if (ENOENT == errno) {
			if (uname(&u) < 0) {
				tcmu_err("uname() failed: %m\n");
			} else {
				tcmu_info("no modules directory '/lib/modules/%s', checking module target_core_user entry in '/sys/modules/'\n",
					  u.release);
				ret = stat("/sys/module/target_core_user", &sb);
				if (!ret) {
					tcmu_dbg("Module target_core_user already loaded\n");
				} else {
					tcmu_err("stat() on '/sys/module/target_core_user' failed: %m\n");
				}
			}
		} else {
			tcmu_err("kmod_module_new_from_lookup() failed to lookup alias target_core_use %m\n");
		}

		kmod_unref(ctx);
		return ret;
	}

	if (!list) {
		tcmu_err("kmod_module_new_from_lookup() failed to find module target_core_user\n");
		kmod_unref(ctx);
		return -ENOENT;
	}

	kmod_list_foreach(itr, list) {
		int state, err;
		struct kmod_module *mod = kmod_module_get_module(itr);

		state = kmod_module_get_initstate(mod);
		switch (state) {
		case KMOD_MODULE_BUILTIN:
			tcmu_info("Module '%s' is builtin\n",
			          kmod_module_get_name(mod));
			break;

		case KMOD_MODULE_LIVE:
			tcmu_dbg("Module '%s' is already loaded\n",
			         kmod_module_get_name(mod));
			break;

		default:
			err = kmod_module_probe_insert_module(mod,
			                               KMOD_PROBE_APPLY_BLACKLIST,
			                               NULL, NULL, NULL, NULL);

			if (err == 0) {
				tcmu_info("Inserted module '%s'\n",
				          kmod_module_get_name(mod));
			} else if (err == KMOD_PROBE_APPLY_BLACKLIST) {
				tcmu_err("Module '%s' is blacklisted\n",
				         kmod_module_get_name(mod));
			} else {
				tcmu_err("Failed to insert '%s'\n",
				         kmod_module_get_name(mod));
			}
			ret = err;
		}
		kmod_module_unref(mod);
	}

	kmod_module_unref_list(list);
	kmod_unref(ctx);

	return ret;
}
