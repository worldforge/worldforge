#include <cstdlib>
#include <iostream>
#include <gpg-error.h>

int main()
{
    gpg_err_init();
    const char * version = gpg_error_check_version(GPG_ERROR_VERSION);
    std::cout << "gpg-error version: " << version << std::endl;
    return EXIT_SUCCESS;
}
