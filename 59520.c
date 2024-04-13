static MagickBooleanType NegateCMYK(Image *image,ExceptionInfo *exception)
{
  ChannelType
    channel_mask;

  MagickBooleanType
    status;

  channel_mask=SetImageChannelMask(image,(ChannelType)(AllChannels &~
    AlphaChannel));
  status=NegateImage(image,MagickFalse,exception);
  (void) SetImageChannelMask(image,channel_mask);
  return(status);
}
