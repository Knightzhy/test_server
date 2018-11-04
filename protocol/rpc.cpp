#include "rpc.h"
namespace rpc{

const uint32_t rpc::Rpc::kMagic = 0x55443322;
const uint32_t rpc::Rpc::kMaximumPacketLength = 1024000;

ssize_t Rpc::GetMessageLength(const void *buffer, size_t length)
{
    if (length < sizeof(Header)) {
        return 0;
    }
    const Header *header = reinterpret_cast<const Header *>(buffer);
    if (header->magic != rpc::Rpc::kMagic) {
        return -1;
    }
    if (header->length > rpc::Rpc::kMaximumPacketLength) {
        return -1;
    }
    return static_cast<ssize_t>(header->length);
}

std::string Rpc::Parse(const void *buffer, size_t length)
{
    return NULL;
}
size_t Rpc::Serialize(void *buffer, const std::string msg, size_t length)
{
    return 0;
}

}
