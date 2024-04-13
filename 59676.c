drop_all_caps (void)
{
  struct __user_cap_header_struct hdr = { _LINUX_CAPABILITY_VERSION_3, 0 };
  struct __user_cap_data_struct data[2] = { { 0 } };

  if (capset (&hdr, data) < 0)
    die_with_error ("capset failed");
}
