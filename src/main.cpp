#include <iostream>

#include <vpConfig.h>

int main(void)
{
    std::cout << "VISP_HAVE_OGRE_VERSION = " << VISP_HAVE_OGRE_VERSION << std::endl;
    const unsigned int ogre_version_api_change = (1<<16 | 11 <<8 | 0);
    std::cout << "(1<<16 | 11 <<8 | 0) = " << ogre_version_api_change << std::endl;

    return 0;
}