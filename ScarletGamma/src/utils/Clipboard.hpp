#pragma once
#include <string>
#include <iostream>

namespace Utils{

	class Clipboard
	{

	public:
		static Clipboard* Instance();

	protected:
		Clipboard();
	private:
		~Clipboard(void);

	public:
		std::string GetClipboardText();
		char* GetClipboardChar();
		void SetClipboardText(const char* _input);
	};
}
