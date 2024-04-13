static void create_core_backtrace(pid_t tid, const char *executable, int signal_no, struct dump_dir *dd)
{
#ifdef ENABLE_DUMP_TIME_UNWIND
    if (g_verbose > 1)
        sr_debug_parser = true;

    char *error_message = NULL;
    char *core_bt = sr_abrt_get_core_stacktrace_from_core_hook(tid, executable,
                                                               signal_no, &error_message);

    if (core_bt == NULL)
    {
        log("Failed to create core_backtrace: %s", error_message);
        free(error_message);
        return;
    }

    dd_save_text(dd, FILENAME_CORE_BACKTRACE, core_bt);
    free(core_bt);
#endif /* ENABLE_DUMP_TIME_UNWIND */
}
