display_augmentation_data (const unsigned char * data, const bfd_size_type len)
{
  bfd_size_type i;

  i = printf (_("  Augmentation data:    "));
  display_data (i, data, len);
}