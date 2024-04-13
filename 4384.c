  void registerKnobParamHandler(
      uint64_t paramId,
      std::function<void(QuicServerConnectionState*, uint64_t)>&& handler) {
    registerTransportKnobParamHandler(paramId, std::move(handler));
  }