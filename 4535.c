void SampleInterleavedLSScan::FindComponentDimensions(void)
{ 
#if ACCUSOFT_CODE
  UBYTE cx;
  
  JPEGLSScan::FindComponentDimensions();

  //
  // Check that all MCU dimensions are 1.
  for(cx = 0;cx < m_ucCount;cx++) {
    class Component *comp = ComponentOf(cx);
    if (comp->MCUHeightOf() != 1 || comp->MCUWidthOf() != 1)
      JPG_THROW(INVALID_PARAMETER,"SampleInterleavedLSScan::FindComponentDimensions",
                "sample interleaved JPEG LS does not support subsampling");
  }
#endif
}