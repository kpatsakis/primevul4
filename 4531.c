  Buf getCryptoStreamData() {
    CHECK(!serverWrites.empty());
    auto cryptoBuf = IOBuf::create(0);
    AckStates ackStates;
    for (auto& serverWrite : serverWrites) {
      auto packetQueue = bufToQueue(serverWrite->clone());
      auto result = clientReadCodec->parsePacket(packetQueue, ackStates);
      auto& parsedPacket = *result.regularPacket();
      for (auto& frame : parsedPacket.frames) {
        if (frame.type() != QuicFrame::Type::ReadCryptoFrame) {
          continue;
        }
        cryptoBuf->prependChain(frame.asReadCryptoFrame()->data->clone());
      }
    }
    return cryptoBuf;
  }