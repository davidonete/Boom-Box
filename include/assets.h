/**
*** ////////////////////////////////////////////
*** /////Autor: Juan Daniel Laserna Condado/////
*** /////Email: S6106112@live.tees.ac.uk   /////
*** /////            2016-2017             /////
*** ////////////////////////////////////////////
**/

#ifndef __ASSETS_H__
#define __ASSETS_H__

#include <string>

namespace ASSETS {
  //@param image: The path from the assets/images folder
  //@return: the correct path (it will be different on Windows or IOS)
  std::string ImagePath(const char* image);

  //@param image: The path from the assets/images folder
  //@return: the correct path (it will be different on Windows or IOS)
  std::string SoundPath(const char* sound);
}

#endif //__ASSETS_H__
