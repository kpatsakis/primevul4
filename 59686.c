set_required_caps (void)
{
  struct __user_cap_header_struct hdr = { _LINUX_CAPABILITY_VERSION_3, 0 };
  struct __user_cap_data_struct data[2] = { { 0 } };

  /* Drop all non-require capabilities */
  data[0].effective = REQUIRED_CAPS_0;
  data[0].permitted = REQUIRED_CAPS_0;
  data[0].inheritable = 0;
  data[1].effective = REQUIRED_CAPS_1;
  data[1].permitted = REQUIRED_CAPS_1;
  data[1].inheritable = 0;
  if (capset (&hdr, data) < 0)
    die_with_error ("capset failed");
}
