  void recvClientFinished(
      bool writes = true,
      folly::SocketAddress* peerAddress = nullptr,
      QuicVersion version = QuicVersion::MVFST) {
    auto finished = IOBuf::copyBuffer("FINISHED");
    auto nextPacketNum = clientNextHandshakePacketNum++;
    auto headerCipher = test::createNoOpHeaderCipher();
    uint64_t offset =
        getCryptoStream(
            *server->getConn().cryptoState, EncryptionLevel::Handshake)
            ->currentReadOffset;
    auto handshakeCipher = test::createNoOpAead();
    auto finishedPacket = packetToBufCleartext(
        createCryptoPacket(
            *clientConnectionId,
            *server->getConn().serverConnectionId,
            nextPacketNum,
            version,
            ProtectionType::Handshake,
            *finished,
            *handshakeCipher,
            0 /* largestAcked */,
            offset),
        *handshakeCipher,
        *headerCipher,
        nextPacketNum);
    deliverData(finishedPacket->clone(), writes, peerAddress);
  }