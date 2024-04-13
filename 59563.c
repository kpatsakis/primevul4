MagickExport void DestroyImageProfiles(Image *image)
{
  if (image->profiles != (SplayTreeInfo *) NULL)
    image->profiles=DestroySplayTree((SplayTreeInfo *) image->profiles);
}
