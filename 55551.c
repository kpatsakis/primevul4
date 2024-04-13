static const char *CompositeOperatorToPSDBlendMode(CompositeOperator op)
{
  const char
    *blend_mode;

  switch (op)
  {
    case ColorBurnCompositeOp:  blend_mode = "idiv";  break;
    case ColorDodgeCompositeOp: blend_mode = "div ";  break;
    case ColorizeCompositeOp:   blend_mode = "colr";  break;
    case DarkenCompositeOp:     blend_mode = "dark";  break;
    case DifferenceCompositeOp: blend_mode = "diff";  break;
    case DissolveCompositeOp:   blend_mode = "diss";  break;
    case ExclusionCompositeOp:  blend_mode = "smud";  break;
    case HardLightCompositeOp:  blend_mode = "hLit";  break;
    case HardMixCompositeOp:    blend_mode = "hMix";  break;
    case HueCompositeOp:        blend_mode = "hue ";  break;
    case LightenCompositeOp:    blend_mode = "lite";  break;
    case LinearBurnCompositeOp: blend_mode = "lbrn";  break;
    case LinearDodgeCompositeOp:blend_mode = "lddg";  break;
    case LinearLightCompositeOp:blend_mode = "lLit";  break;
    case LuminizeCompositeOp:   blend_mode = "lum ";  break;
    case MultiplyCompositeOp:   blend_mode = "mul ";  break;
    case OverCompositeOp:       blend_mode = "norm";  break;
    case OverlayCompositeOp:    blend_mode = "over";  break;
    case PinLightCompositeOp:   blend_mode = "pLit";  break;
    case SaturateCompositeOp:   blend_mode = "sat ";  break;
    case ScreenCompositeOp:     blend_mode = "scrn";  break;
    case SoftLightCompositeOp:  blend_mode = "sLit";  break;
    case VividLightCompositeOp: blend_mode = "vLit";  break;
    default:                    blend_mode = "norm";  break;
  }
  return(blend_mode);
}
