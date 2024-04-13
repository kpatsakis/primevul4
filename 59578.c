MagickExport StringInfo *RemoveImageProfile(Image *image,const char *name)
{
  StringInfo
    *profile;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (image->profiles == (SplayTreeInfo *) NULL)
    return((StringInfo *) NULL);
  if (LocaleCompare(name,"icc") == 0)
    {
      /*
        Continue to support deprecated color profile for now.
      */
      image->color_profile.length=0;
      image->color_profile.info=(unsigned char *) NULL;
    }
  if (LocaleCompare(name,"iptc") == 0)
    {
      /*
        Continue to support deprecated IPTC profile for now.
      */
      image->iptc_profile.length=0;
      image->iptc_profile.info=(unsigned char *) NULL;
    }
  WriteTo8BimProfile(image,name,(StringInfo *) NULL);
  profile=(StringInfo *) RemoveNodeFromSplayTree((SplayTreeInfo *)
    image->profiles,name);
  return(profile);
}
