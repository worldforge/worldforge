#include <iostream>
#include <gcrypt.h>

int main()
{
    std::cout << "gcrypt version: "<< gcry_check_version(GCRYPT_VERSION) << std::endl;
    return 0;
}
