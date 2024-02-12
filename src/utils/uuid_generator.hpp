#include <uuid.h>

class UUIDGenerator {
    public:
        explicit UUIDGenerator() = default;

        std::string generate(){
            std::random_device dev;
            std::mt19937 rng(dev());
            auto id = uuids::uuid_random_generator{rng}();

            return uuids::to_string(id);
        }
};
