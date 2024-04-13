ModuleExport MagickBooleanType ReadPSDLayers(Image *image,
  const ImageInfo *image_info,const PSDInfo *psd_info,
  const MagickBooleanType skip_layers,ExceptionInfo *exception)
{
  PolicyDomain
    domain;

  PolicyRights
    rights;

  domain=CoderPolicyDomain;
  rights=ReadPolicyRights;
  if (IsRightsAuthorized(domain,rights,"PSD") == MagickFalse)
    return(MagickFalse);
  return(ReadPSDLayersInternal(image,image_info,psd_info,skip_layers,
    exception));
}
