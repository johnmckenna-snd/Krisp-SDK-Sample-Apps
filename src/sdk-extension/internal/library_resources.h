#pragma once

#include <memory>


namespace KrispVoiceSDK
{

class LibraryResources
{
public:
	LibraryResources();
	~LibraryResources();
};

std::shared_ptr<LibraryResources> getLibrary();

}
