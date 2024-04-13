  void recvClientHello(
      bool writes = true,
      QuicVersion version = QuicVersion::MVFST) {
    auto chlo = IOBuf::copyBuffer("CHLO");
    auto nextPacketNum = clientNextInitialPacketNum++;
    auto aead = getInitialCipher(version);
    auto headerCipher = getInitialHeaderCipher(version);
    auto initialPacket = packetToBufCleartext(
        createInitialCryptoPacket(
            *clientConnectionId,
            *initialDestinationConnectionId,
            nextPacketNum,
            version,
            *chlo,
            *aead,
            0 /* largestAcked */),
        *aead,
        *headerCipher,
        nextPacketNum);
    deliverData(initialPacket->clone(), writes);
  }