do_cleanup (void)
{
  support_chdir_toolong_temp_directory (base);
  TEST_VERIFY_EXIT (rmdir (MOUNT_NAME) == 0);
  free (base);
}