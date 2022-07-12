#ifndef __COMMON_TYPES_H__
#define __COMMON_TYPES_H__

namespace common {

#pragma pack(push)
#pragma pack(1)
struct User {
    int attr1;
    int attr2;
    bool attr3;
};
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
struct Address {
    int attr1;
    char attr2[100];
    bool attr3;
};
#pragma pack(pop)

} // end namespace common

#endif
