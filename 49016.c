static void sighup_handler(const int s G_GNUC_UNUSED) {
        is_sighup_caught = 1;
}
