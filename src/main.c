#include "i2c_adapter.h"

int main() {
    int fd = i2c_create_adapter(0);
    return 0;
}
