#ifndef __RPC_H__
#define __RPC_H__
#include <string>

namespace rpc{

#pragma pack(push)
#pragma pack(1)
struct Header{
uint32_t magic;
uint32_t length;
};

struct Message{
uint32_t magic;
uint32_t length;
unsigned char msg[1];
};
#pragma pack(pop)

class Rpc{
public:
Rpc(){}
~Rpc(){}
static ssize_t GetMessageLength(const void * buffer, size_t length);
static std::string Parse(const void *buffer, size_t length);
static size_t Serialize(void *buffer, const std::string msg, size_t length);

static const uint32_t kMagic;
static const uint32_t kMaximumPacketLength;
};


}
#endif
