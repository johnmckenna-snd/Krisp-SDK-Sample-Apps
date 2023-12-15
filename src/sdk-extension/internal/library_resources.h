#pragma once

#include <memory>


namespace KrispVoiceSdk
{

class LibraryResources
{
public:
	LibraryResources();
	~LibraryResources();
};

std::shared_ptr<LibraryResources> getLibrary();

}
