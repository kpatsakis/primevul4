static const char *ModeToString(PSDImageType type)
{
  switch (type)
  {
    case BitmapMode: return "Bitmap";
    case GrayscaleMode: return "Grayscale";
    case IndexedMode: return "Indexed";
    case RGBMode: return "RGB";
    case CMYKMode:  return "CMYK";
    case MultichannelMode: return "Multichannel";
    case DuotoneMode: return "Duotone";
    case LabMode: return "L*A*B";
    default: return "unknown";
  }
}
