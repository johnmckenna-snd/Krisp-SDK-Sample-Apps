#pragma once

#include <memory>


namespace KrispAudioSDK
{

class LibraryResources
{
public:
	LibraryResources();
	~LibraryResources();
};

std::shared_ptr<LibraryResources> get_library();

}
