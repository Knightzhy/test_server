#include <iostream>

#include "google/protobuf/descriptor.h"
#include "google/protobuf/io/coded_stream.h"

#include "google/protobuf/descriptor.pb.h"

#include "google/protobuf/dynamic_message.h"

#include "google/protobuf/compiler/importer.h"

using namespace google::protobuf;

using namespace google::protobuf::compiler;

int main(int argc,const char *argv[])

{
    DiskSourceTree sourceTree;

    //look up .proto file in current directory

    sourceTree.MapPath("","/home/knightwu/projects/proto/");
    sourceTree.MapPath("","/home/knightwu/projects/thirdparty/staging/include");
    Importer importer(&sourceTree, NULL);
    importer.Import("xx.proto");
    google::protobuf::io::ZeroCopyInputStream * stream = sourceTree.Open("helloworld.proto");
    google::protobuf::io::CodedInputStream codeInputStream(stream);
    const FileDescriptor * file_descriptor = importer.Import("helloworld.proto");
    const Descriptor *descriptor = file_descriptor->pool()->FindMessageTypeByName("helloworld.HelloRequest");
  //  const Descriptor *descriptor = file_descriptor->pool()->FindMessageTypeByName("google.protobuf.MessageOptions");
    if (descriptor) {
        printf("find\n");
        std::cout << descriptor->DebugString() << std::endl;
        const google::protobuf::MessageOptions &options = descriptor->options();
        std::cout << "options :" << options.DebugString() << std::endl;
        if ( file_descriptor->pool() == options.GetDescriptor()->file()->pool() ){
            printf("pool equal\n");
        } else {
            printf("pool not equal\n");
        }
        const google::protobuf::DescriptorPool* pool = options.GetDescriptor()->file()->pool();
        const google::protobuf::Descriptor* option_descriptor = pool->FindMessageTypeByName(options.GetDescriptor()->full_name());
        if (option_descriptor == NULL) {
            printf("not find option descriptor\n");
        }
        google::protobuf::DynamicMessageFactory factory(file_descriptor->pool());
        codeInputStream.SetExtensionRegistry(file_descriptor->pool(), &factory);
        google::protobuf::Message *dynamic_options = factory.GetPrototype(descriptor)->New();
        if (dynamic_options->ParseFromString(options.SerializeAsString())) {
            const google::protobuf::Reflection* reflection = options.GetReflection();
            std::vector<const google::protobuf::FieldDescriptor*> fields;
            reflection->ListFields(options, &fields);
            std::cout << "options:" << dynamic_options->DebugString() << std::endl;
            std::cout << "fields.size:" << fields.size() << std::endl;
            for (int i = 0; i < fields.size(); i++) {
            }
        } else {
            printf("Found invalid proto option data for: \n");
        }
    } else {
        printf("not find\n");
    }


    /*

    const Descriptor *descriptor = importer.pool()->
        FindMessageTypeByName("helloworld.HelloRequest");
    if (descriptor) {
        std::cout << descriptor->DebugString() << std::endl;
    } else {
        printf("not find\n");
    }
    */

    /*
    const FileDescriptor * file_descriptor = importer.pool()->FindFileByName("helloworld.proto");
    if (file_descriptor) {
        printf("find\n");
        std::cout << "extension_count: " << file_descriptor->extension_count() << std::endl;
        std::cout << "extension:" << file_descriptor->extension(0)->full_name() << std::endl;
        std::cout << "extension tag number:" << file_descriptor->extension(0)->number() << std::endl;
        std::cout << "find extension by name, address:" << file_descriptor->FindExtensionByName("my_option") << std::endl;
        for (int i=0; i< file_descriptor->message_type_count(); i++) {
            auto factory = new  google::protobuf::DynamicMessageFactory;
            const Descriptor *descriptor = file_descriptor->message_type(i);
            if (!descriptor) {
                continue;
            }
            if (descriptor->full_name()!="helloworld.HelloRequest") {
                continue;
            }

            factory->GetPrototype(descriptor);
            const Message *prototype =
            std::cout << "message name:" << descriptor->full_name() << std::endl;
            std::cout << "message string:" << descriptor->DebugString() << std::endl;
            std::cout << "range count:" << descriptor->extension_range_count() << std::endl;
            std::cout << "count:" << descriptor->extension_count() << std::endl;
            const google::protobuf::MessageOptions &options = descriptor->options();
            std::cout << "options :" << options.DebugString() << std::endl;
            std::cout << "GetDescriptor:" << options.GetDescriptor()->DebugString() << std::endl;
            std::cout << "GetDescriptor field count:" << options.descriptor()->field_count() << std::endl;
            for (int j=0; j< descriptor->field_count(); j++) {
                const FieldDescriptor * field_descriptor = descriptor->field(j);
                if (!field_descriptor) {
                    continue;
                }
                std::cout << field_descriptor->full_name() << std::endl;
                std::cout << field_descriptor->DebugString() << std::endl;
            }
        }
    } else {
        printf("not find\n");
    }
    std::cout << "========================" << std::endl;
    auto descriptor_pool = google::protobuf::DescriptorPool::generated_pool();
    if (!descriptor_pool) {
        std::cout << "err generated_pool()" << std::endl;
        return 1;
    }
    auto descriptor = descriptor_pool->FindMessageTypeByName("google.protobuf.FileOptions");
    if (!descriptor) {
        std::cout << "err generated_pool()->FindMessageTypeByName" << std::endl;
        return 1;
    }
    auto prototype = google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
    if (!prototype) {
        std::cout << "err generated_factory()->GetPrototype" << std::endl;
        return 1;
    }
    auto instance = prototype->New();
    if (!instance) {
        std::cout << "err prototype->New" << std::endl;
        return 1;
    }
    */
}
