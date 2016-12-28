#include "Assets.h"

//Windows Paths
#define WIN_IMAGE_PATH "../../../data/images/"
#define WIN_SOUND_PATH "../../../data/sounds/"

//IOS Paths
#define IOS_IMAGE_PATH "data/images/"
#define IOS_SOUND_PATH "data/sounds/"

//Path on the different OS targets
#ifdef _WIN32
#define IMAGE_PATH WIN_IMAGE_PATH
#define SOUND_PATH WIN_SOUND_PATH
#elif __APPLE__
#define IMAGE_PATH IOS_IMAGE_PATH
#define SOUND_PATH IOS_SOUND_PATH
#endif

std::string ASSETS::ImagePath(const char* image) {
  std::string path = IMAGE_PATH;
  return path.append(image);
}

std::string ASSETS::SoundPath(const char* sound) {
  std::string path = SOUND_PATH;
  return path.append(sound);
}
