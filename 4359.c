  std::shared_ptr<FizzServerQuicHandshakeContext> getFizzServerContext() {
    if (!fizzServerContext) {
      fizzServerContext = FizzServerQuicHandshakeContext::Builder()
                              .setFizzServerContext(createServerCtx())
                              .build();
    }

    return fizzServerContext;
  }