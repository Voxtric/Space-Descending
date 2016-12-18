// Author Chris Rook
// Version 12.10
// 6/12/12
// First write
// Defines KeyboardBuffer class

#pragma once
// Simple class intended to store keyboard input for the purposes of typing, etc
// To use the character for typed input, I suggest you first "Clear" the buffer,
// display the result of GetBuffer each frame.
// Once "Enter" is pressed, extract the contents and clear the buffer

class KeyboardBuffer
{
private:
	static const int BUFFERSIZE=256;
	char buffer[BUFFERSIZE];
	int numKeys;
	bool enterPressed;

public:
	// Creates an empty buffer
	KeyboardBuffer();

	// Adds the specified character to the buffer. If the buffer
	// reaches BUFFERSIZE-1 characters, the buffer is emptied first
	void Add(char key);

	// Clears the buffer
	void Clear();

	// Returns a null-terminated string with the characters inputted
	// since the buffer was last cleared. 
	// In normal use, programmer should call "Clear" once the buffer has been
	// used.
	char* GetBuffer();

	// Returns true if the enter key has been pressed since the last time
	// the buffer was cleared.
	bool EnterPressed();
};