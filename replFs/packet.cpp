/*
 * packet.cpp
 *
 *  Created on: May 17, 2014
 *      Author: songhan
 */
#include "packet.h"

PacketBase::PacketBase(uint8_t type, uint8_t nodeType, uint32_t GUID,
                       uint32_t fd, uint32_t seqNum, uint32_t transNum) {
  this->type = type;
  this->nodeType = nodeType;
  this->checkSum = 0;
  this->GUID = GUID;
  this->fd = fd;
  this->seqNum = seqNum;
  this->transNum = transNum;
}

OpenPkt::OpenPkt(uint32_t GUID, uint32_t fd, uint32_t seqNum, uint32_t transNum,
                 char* fileName)
    : PacketBase(OPEN, CLIENT, GUID, fd, seqNum, transNum) {
  int i;
  for (i = 0; i < strlen(fileName) && i < MAX_FILE_NAME - 1; i++) {
    this->fileName[i] = fileName[i];
  }
  fileName[i] = '\0';
}

OpenAckPkt::OpenAckPkt(uint32_t GUID, uint32_t fd, uint32_t seqNum,
                       uint32_t transNum, bool status)
    : PacketBase(OPEN_ACK, SERVER, GUID, fd, seqNum, transNum) {
  this->status = status;
}

WriteBlockPkt::WriteBlockPkt(uint32_t GUID, uint32_t fd, uint32_t seqNum,
                             uint32_t transNum, int blockID, int offset,
                             int size, uint8_t* payload)
    : PacketBase(WRITE_BLOCK, CLIENT, GUID, fd, seqNum, transNum) {
  this->blockID = blockID;
  this->offset = offset;
  this->size = size;
  for (int i = 0; i < size; i++) {
    this->payload[i] = payload[i];
  }

}

CommitVotingPkt::CommitVotingPkt(uint32_t GUID, uint32_t fd, uint32_t seqNum,
                                 uint32_t transNum, int totalPending)
    : PacketBase(COMMIT_VOTING, CLIENT, GUID, fd, seqNum, transNum) {
  this->totalPending = totalPending;
}

CommitVotingSuccessPkt::CommitVotingSuccessPkt(uint32_t GUID, uint32_t fd,
                                               uint32_t seqNum,
                                               uint32_t transNum)
    : PacketBase(COMMIT_VOTING_SUCCESS, SERVER, GUID, fd, seqNum, transNum) {
}

CommitVotingResendPkt::CommitVotingResendPkt(uint32_t GUID, uint32_t fd,
                                             uint32_t seqNum, uint32_t transNum,
                                             int totalMissing, int* MissingIDs)
    : PacketBase(COMMIT_VOTING_RESEND, SERVER, GUID, fd, seqNum, transNum) {
  this->totalMissing = totalMissing;
  for (int i = 0; i < totalMissing; i++) {
    this->vectorMissingID.push_back(MissingIDs[i]);
  }
}

CommitFinalPkt::CommitFinalPkt(uint32_t GUID, uint32_t fd, uint32_t seqNum,
                               uint32_t transNum)
    : PacketBase(COMMIT_FINAL, CLIENT, GUID, fd, seqNum, transNum) {
}

CommitFinalReplyPkt::CommitFinalReplyPkt(uint32_t GUID, uint32_t fd,
                                         uint32_t seqNum, uint32_t transNum,
                                         bool status)
    : PacketBase(COMMIT_FINAL_REPLY, SERVER, GUID, fd, seqNum, transNum) {
  this->status = status;
}

AbortPkt::AbortPkt(uint32_t GUID, uint32_t fd, uint32_t seqNum,
                   uint32_t transNum)
    : PacketBase(ABORT, CLIENT, GUID, fd, seqNum, transNum) {
}

ClosePkt::ClosePkt(uint32_t GUID, uint32_t fd, uint32_t seqNum,
                   uint32_t transNum, int totalPending)
    : PacketBase(CLOSE, CLIENT, GUID, fd, seqNum, transNum) {
  this->totalPending = totalPending;
}

void PacketBase::serialize(std::ostream& stream) const {
  stream.write(reinterpret_cast<const char *>(&type), sizeof(type));
  stream.write(reinterpret_cast<const char *>(&nodeType), sizeof(nodeType));
  stream.write(reinterpret_cast<const char *>(&checkSum), sizeof(checkSum));
  stream.write(reinterpret_cast<const char *>(&GUID), sizeof(GUID));
  stream.write(reinterpret_cast<const char *>(&fd), sizeof(fd));
  stream.write(reinterpret_cast<const char *>(&seqNum), sizeof(seqNum));
  stream.write(reinterpret_cast<const char *>(&transNum), sizeof(transNum));
}

void PacketBase::deserialize(std::istream& stream) {
  stream.read(reinterpret_cast<char *>(&type), sizeof(type));
  stream.read(reinterpret_cast<char *>(&nodeType), sizeof(nodeType));
  stream.read(reinterpret_cast<char *>(&checkSum), sizeof(checkSum));
  stream.read(reinterpret_cast<char *>(&GUID), sizeof(GUID));
  stream.read(reinterpret_cast<char *>(&fd), sizeof(fd));
  stream.read(reinterpret_cast<char *>(&seqNum), sizeof(seqNum));
  stream.read(reinterpret_cast<char *>(&transNum), sizeof(transNum));
}

void OpenPkt::serialize(std::ostream& stream) const {
  PacketBase::serialize(stream);
  stream.write(reinterpret_cast<const char *>(&fileName), sizeof(fileName));

}

void OpenPkt::deserialize(std::istream& stream) {
  PacketBase::deserialize(stream);
  stream.read(reinterpret_cast<char *>(&fileName), sizeof(fileName));

}

void OpenAckPkt::serialize(std::ostream& stream) const {
  PacketBase::serialize(stream);
  stream.write(reinterpret_cast<const char *>(&status), sizeof(status));

}

void OpenAckPkt::deserialize(std::istream& stream) {
  PacketBase::deserialize(stream);
  stream.read(reinterpret_cast<char *>(&status), sizeof(status));
}

void WriteBlockPkt::serialize(std::ostream& stream) const {
  PacketBase::serialize(stream);
  stream.write(reinterpret_cast<const char *>(&blockID), sizeof(blockID));
  stream.write(reinterpret_cast<const char *>(&offset), sizeof(offset));
  stream.write(reinterpret_cast<const char *>(&size), sizeof(size));
  stream.write(reinterpret_cast<const char *>(&payload),
               size * sizeof(uint8_t));
}

void WriteBlockPkt::deserialize(std::istream& stream) {
  PacketBase::deserialize(stream);
  stream.read(reinterpret_cast<char *>(&blockID), sizeof(blockID));
  stream.read(reinterpret_cast<char *>(&offset), sizeof(offset));
  stream.read(reinterpret_cast<char *>(&size), sizeof(size));
  stream.read(reinterpret_cast<char *>(&payload), size * sizeof(uint8_t));
}

void CommitVotingPkt::serialize(std::ostream& stream) const {
  PacketBase::serialize(stream);
  stream.write(reinterpret_cast<const char *>(&totalPending),
               sizeof(totalPending));
}

void CommitVotingPkt::deserialize(std::istream& stream) {
  PacketBase::deserialize(stream);
  stream.read(reinterpret_cast<char *>(&totalPending), sizeof(totalPending));
}

void CommitVotingSuccessPkt::serialize(std::ostream& stream) const {
  PacketBase::serialize(stream);
}

void CommitVotingSuccessPkt::deserialize(std::istream& stream) {
  PacketBase::deserialize(stream);
}

void CommitVotingResendPkt::serialize(std::ostream& stream) const {
  PacketBase::serialize(stream);
  stream.write(reinterpret_cast<const char *>(&totalMissing),
               sizeof(totalMissing));
  for (int i = 0; i < totalMissing; i++) {
    stream.write(reinterpret_cast<const char *>(&vectorMissingID[i]),
                 sizeof(int));
  }
}

void CommitVotingResendPkt::deserialize(std::istream& stream) {
  PacketBase::deserialize(stream);
  stream.read(reinterpret_cast<char *>(&totalMissing), sizeof(totalMissing));
  int missingID;
  for (int i = 0; i < totalMissing; i++) {
    stream.read(reinterpret_cast<const char *>(&missingID]),
        sizeof(int));
    vectorMissingID.push_back(missingID);
  }
}

void CommitFinalPkt::serialize(std::ostream& stream) const {
  PacketBase::serialize(stream);
}

void CommitFinalPkt::deserialize(std::istream& stream) {
  PacketBase::deserialize(stream);
}

void CommitFinalReplyPkt::serialize(std::ostream& stream) const {
  PacketBase::serialize(stream);
  stream.write(reinterpret_cast<const char *>(&status), sizeof(status));
}

void CommitFinalReplyPkt::deserialize(std::istream& stream) {
  PacketBase::deserialize(stream);
  stream.read(reinterpret_cast<char *>(&status), sizeof(status));
}

void AbortPkt::serialize(std::ostream& stream) const {
  PacketBase::serialize(stream);
}

void AbortPkt::deserialize(std::istream& stream) {
  PacketBase::deserialize(stream);
}

void ClosePkt::serialize(std::ostream& stream) const {
  PacketBase::serialize(stream);
  stream.write(reinterpret_cast<const char *>(&totalPending),
               sizeof(totalPending));
}

void ClosePkt::deserialize(std::istream& stream) {
  PacketBase::deserialize(stream);
  stream.read(reinterpret_cast<char *>(&totalPending), sizeof(totalPending));
}
