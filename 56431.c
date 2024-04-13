static void MSLPopImage(MSLInfo *msl_info)
{
  if (msl_info->number_groups != 0)
    return;
  if (msl_info->image[msl_info->n] != (Image *) NULL)
    msl_info->image[msl_info->n]=DestroyImage(msl_info->image[msl_info->n]);
  msl_info->attributes[msl_info->n]=DestroyImage(
    msl_info->attributes[msl_info->n]);
  msl_info->image_info[msl_info->n]=DestroyImageInfo(
    msl_info->image_info[msl_info->n]);
  msl_info->n--;
}
