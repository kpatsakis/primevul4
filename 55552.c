static inline CompressionType ConvertPSDCompression(
  PSDCompressionType compression)
{
  switch (compression)
  {
    case RLE:
      return RLECompression;
    case ZipWithPrediction:
    case ZipWithoutPrediction:
      return ZipCompression;
    default:
      return NoCompression;
  }
}
