#include <string.h>
#include "rpc.h"
namespace rpc{

const uint32_t Rpc::kMagic = 0x55443322;
const uint32_t Rpc::kMaximumPacketLength = 1024000;

ssize_t Rpc::GetMessageLength(const void *buffer, size_t length)
{
    if (length < sizeof(Header)) {
        return 0;
    }
    const Header *header = reinterpret_cast<const Header *>(buffer);
    if (header->magic != Rpc::kMagic) {
        return -1;
    }
    if (header->length > Rpc::kMaximumPacketLength) {
        return -1;
    }
    return static_cast<ssize_t>(header->length);
}

size_t Rpc::GetMessageLength(const std::string &msg)
{
    printf("message=%d, msg.length=%d\n",
            (int)sizeof(Message), strlen(msg.c_str()));
    return sizeof(Message) + strlen(msg.c_str());
}

size_t Rpc::GetHeader()
{
    return sizeof(Header);
}

std::string Rpc::Parse(const void *buffer, size_t length)
{
    const Message *message = reinterpret_cast<const Message *>(buffer);
    printf("Message:msg=%s, magic=%x, length=%d\n",
            message->msg, message->magic, message->length);
    std::string msg = message->msg;
    return msg;
}

int Rpc::Parse(const void *buffer, size_t length, std::string &msg)
{
    const Message *message = reinterpret_cast<const Message *>(buffer);
    printf("Message:msg=%s, magic=%x, length=%d\n",
            message->msg, message->magic, message->length);
    if (message->length > length) {
        printf("Error, message->length=%d, length=%d",
                message->length, (int)length);
        return -1;
    }
    if (strlen(message->msg) > length) {
        printf("Error, strlen(message->msg=%d, length=%d",
                strlen(message->msg), (int)length);
        return -2;
    }
    msg = message->msg;
    return 0;
}

size_t Rpc::Serialize(void *buffer, const std::string &msg)
{
    if (strlen(msg.c_str()) + sizeof(Message)> Rpc::kMaximumPacketLength){
        return 0;
    }

    Message *message = reinterpret_cast<Message *>(buffer);
    message->magic = Rpc::kMagic;
    message->length = (uint32_t)sizeof(Message) + (uint32_t)strlen(msg.c_str());
    strcpy(message->msg, msg.c_str());
    return (size_t)message->length;
}

}
