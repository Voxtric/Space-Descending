#include "keyboardbuffer.h"
#include <windows.h>			// For VK_RETURN


KeyboardBuffer::KeyboardBuffer()
{
	buffer[0] = '\0';
	numKeys =0;
	enterPressed = false;
}

void KeyboardBuffer::Add(char key)
{
	if(key ==VK_RETURN)
		enterPressed = true;
	if(numKeys>=BUFFERSIZE-2)
	{
		Clear();
	}
	buffer[numKeys] = key;
	buffer[numKeys+1] ='\0';
	++numKeys;
}
void KeyboardBuffer::Clear()
{
	buffer[0] = '\0';
	numKeys =0;
	enterPressed = false;
}

char* KeyboardBuffer::GetBuffer()
{
	return buffer;
}

	// Returns true if the enter key has been pressed since the last time
	// the buffer was cleared.
bool KeyboardBuffer::EnterPressed()
{
	return enterPressed;
}