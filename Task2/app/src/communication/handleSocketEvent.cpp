#include <sstream>
#include "communication/handleSocketEvent.hpp"
#include "util/unix.hpp"

void handleSocketEvent(const pollfd& pollDescriptor,
                       bool drainPacketQueue,
                       std::mutex& packetQueueMutex,
                       std::vector<std::unique_ptr<Packet>>& packetQueue,
                       const std::function<void(std::unique_ptr<Packet>)>& packetCallback) {
    if(pollDescriptor.revents & POLLHUP || pollDescriptor.revents & POLLERR) {
        std::stringstream errorString;
        errorString << "Error occurred in connection: " << pollDescriptor.fd << ERROR_LOG;
        throw std::runtime_error(errorString.str());
    }

    // The socketDescriptor has unprocessed inbound data
    if(pollDescriptor.revents & POLLIN) {
        const int READ_CHUNK_SIZE = 1024;
        std::vector<uint8_t> bytes;

        ssize_t bytesReadCount;
        do {
            std::array<uint8_t, READ_CHUNK_SIZE> readArray = {0};
            bytesReadCount = read(pollDescriptor.fd, static_cast<void*>(readArray.data()), readArray.size());
            if(bytesReadCount > 0) {
                bytes.insert(bytes.end(), readArray.begin(), readArray.begin() + bytesReadCount);
            }
        } while (bytesReadCount == READ_CHUNK_SIZE);

        if(bytesReadCount == 0) {
            // A POLLIN event with a read size of 0, means that our client has disconnected
            throw std::runtime_error("No data found in inbound packet");
        } else if (bytesReadCount < 0) {
            std::stringstream errorString;
            errorString << "Error occurred while attempting to read packet" << ERROR_LOG;
            throw std::runtime_error(errorString.str());
        }

        auto packet = Packet::parsePacket(bytes);
        packetCallback(std::move(packet));
    }

    // The socketDescriptor is ready to send data and the packetQueue contains data
    if(pollDescriptor.revents & POLLOUT && !packetQueue.empty()) {
        std::lock_guard<std::mutex> packetQueueGuard(packetQueueMutex);
        for (const auto &packet: packetQueue) {
            const auto bytes = packet->getBytes();
            const auto bytesWritten = write(pollDescriptor.fd, bytes.data(), bytes.size());
            if (bytesWritten < 0) {
                std::stringstream errorString;
                errorString << "Error occurred while attempting to write packet" << ERROR_LOG;
                throw std::runtime_error(errorString.str());
            }
        }
        if(drainPacketQueue) packetQueue.clear();
    }
}