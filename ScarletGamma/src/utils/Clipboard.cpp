#include "Clipboard.hpp"
#include <windows.h>

namespace Utils
{
	Clipboard::Clipboard(){

	}


	Clipboard::~Clipboard(void)
	{
	}


	Clipboard* Clipboard::Instance(){
		static Clipboard Instance;
		return &Instance;
	}

	std::string Clipboard::GetClipboardText()
	{
		// Lock the handle to get the actual text pointer
		char * pszText = GetClipboardChar();

		// Save text in a string class instance
		std::string text( pszText );

		return text;
	}


	char* Clipboard::GetClipboardChar()
	{
		// Try opening the clipboard
		if (! OpenClipboard(nullptr))
			std::cerr<<"Failed to open clipboard";

		// Get handle of clipboard object for ANSI text
		HANDLE hData = GetClipboardData(CF_TEXT);
		if (hData == nullptr)
			std::cerr<<"Failed to get clipboard data";

		// Lock the handle to get the actual text pointer
		char * pszText = static_cast<char*>( GlobalLock(hData) );
		if (pszText == nullptr)
			std::cerr<<"Failed to cast data to string";

		// Release the lock
		GlobalUnlock( hData );

		// Release the clipboard
		CloseClipboard();

		return pszText;

	}

	void Clipboard::SetClipboardText(const char* _input){
		const size_t len = strlen(_input) + 1;

		HGLOBAL hMem =  GlobalAlloc(GMEM_MOVEABLE, len);

		memcpy(GlobalLock(hMem), _input, len);
		GlobalUnlock(hMem);
		// Try opening the clipboard
		if (! OpenClipboard(nullptr))
			std::cerr<<"Failed to open clipboard";
		if (!EmptyClipboard())
			std::cerr<<"Failed to clear clipboard";
		HANDLE hData = SetClipboardData(CF_TEXT, hMem);
		if (hData == nullptr)
			std::cerr<<"Failed to set clipboard data";
		CloseClipboard();
	}
}
