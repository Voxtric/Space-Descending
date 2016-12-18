// mysoundengine.cpp	Version 10		9/5/05
// The definition file for the methods in MySoundEngine, declared in mysoundengine.h
#define DSBCAPS_CTRLDEFAULT 0x000000E0

#include "mysoundengine.h"
#include "errorlogger.h"
#include "dxerr.h"

#define ERRORSTRING(x) DXGetErrorDescription(x)

MySoundEngine* MySoundEngine::instance=nullptr;

MySoundEngine::MySoundEngine(HWND hwnd)
{
	// The first sound loaded will have a SoundIndex value of 1
	m_NextSoundIndex = 1;

	// Create an empty sound to be returned when a requested sound
	// is not found in the map
	MySound temp;
	temp.lpSoundBuffer=nullptr;
	temp.m_sourceFileName=L"Unknown sound";
	m_MySoundList.insert(std::pair<SoundIndex, MySound>(0, temp));

	// Initialise dsound
	HRESULT err;
	if (FAILED(DirectSoundCreate8(&DSDEVID_DefaultPlayback, &lpds, NULL)))
	{
		ErrorLogger::Writeln(L"Failed to create sound player");
		lpds=nullptr;
	}

	// Set cooperative level and check for error
	err=lpds->SetCooperativeLevel(hwnd, DSSCL_NORMAL);

	if (FAILED(err))	// If failed to set cooperative level
	{
		ErrorLogger::Writeln(L"Failed to set cooperative level\n");
		lpds->Release();
		lpds=nullptr;
		ErrorLogger::Writeln(ErrorString(err));
	}
}

MySoundEngine::~MySoundEngine()
{
	// Unload and release sound buffers
	UnloadAllSounds();
	// Release DirectSound
	Release();
}

ErrorType MySoundEngine::Release()
{
	if (lpds)			// If not already null
	{
		lpds->Release();
		lpds=nullptr;
		return SUCCESS;
	}
	return FAILURE;
}

MySoundEngine* MySoundEngine::Start(HWND hwnd)
{
	if(instance)
	{
		instance->Terminate();
	}
	instance = new MySoundEngine(hwnd);
	return instance;
}

MySoundEngine* MySoundEngine::GetInstance()
{
	return instance;
}

ErrorType MySoundEngine::Terminate()
{
	if(instance)
	{
		delete instance;
		instance=nullptr;
		return SUCCESS;
	}
	else
		return FAILURE;
}

const wchar_t* MySoundEngine::ErrorString(HRESULT err)
{
	// Returns an error string from DirectX
	return ERRORSTRING(err);
}

MySoundEngine::MySound& MySoundEngine::FindSound(SoundIndex sound)
{
	// Find a sound in the map
	std::map<SoundIndex, MySound>::iterator it = m_MySoundList.find(sound);
	if(it  == m_MySoundList.end())			// Not found
	{
		ErrorLogger::Writeln(L"SoundIndex not found");
		// Return the sound at position 0 - has an empty sound buffer
		return m_MySoundList[0];
	}
	return it->second;
}

SoundIndex MySoundEngine::LoadWav(wchar_t* filename)
// CAUTION - Multiple early returns
{
	if(!lpds)
	{
		ErrorLogger::Writeln(L"Cannot load a sound wave - No pointer to DirectSound.");
		return 0;			// Early return  **
	}

	MySound temp;

	temp.lpSoundBuffer=nullptr;
	temp.m_sourceFileName = filename;

	DSBUFFERDESC dsbd;			// "Order form" for the sound
	WAVEFORMATEX formatdesc;	// Description of the format	
	HMMIO hWaveFile;		// Handle to the wave file
	MMCKINFO parent;		// A parent chunk (wav file data chunks)
	MMCKINFO child;			// A child chunk (wav file data chunks)

	UCHAR *tempBuffer;		// Pointer to a buffer to temporarily store sound
	UCHAR *tempPtr1;		// Pointer to first part of sound buffer
	UCHAR *tempPtr2;		// Pointer to second part of sound buffer
	DWORD length1;			// Length of first part of sound buffer
	DWORD length2;			// Length of second part of sound buffer

	// ***************************************************************
	// Most of what follows is some fairly complicated bits that
	// open a pcm wave file, and read the contents into the
	// directX buffer.

	// Chunk info initialised

	parent.ckid			= (FOURCC)0;
	parent.cksize		= 0;
	parent.fccType		= (FOURCC)0;
	parent.dwDataOffset	= 0;
	parent.dwFlags		= 0;

	child=parent;

	// Open the wav file

	hWaveFile = mmioOpen(filename, NULL, MMIO_READ|MMIO_ALLOCBUF);	

	if (!hWaveFile)			// If file could not open
	{
		ErrorLogger::Write(L"Failed to open sound file ");
		ErrorLogger::Writeln(filename);
		return 0;			// Early return  **
	}

	// Find the wave section
	// What is it with sound engineers? Is is because they are musicians
	// that they like using meaningless terms like "descend"?
	parent.fccType=mmioFOURCC('W','A','V','E');

	if (mmioDescend(hWaveFile, &parent, NULL, MMIO_FINDRIFF))
	{
		ErrorLogger::Write(L"Couldn't find wave section in wave file ");
		ErrorLogger::Writeln(filename);

		mmioClose(hWaveFile,0);			// Error - close the wave file
		return 0;			// Early return  **
	}

	// Find the format section
	child.ckid=mmioFOURCC('f','m','t',' ');
	if (mmioDescend(hWaveFile, &child,&parent, 0)!=MMSYSERR_NOERROR)
	{
		ErrorLogger::Write(L"Couldn't find format section in wave file ");
		ErrorLogger::Writeln(filename);

		mmioClose(hWaveFile,0);			// Error - close the wave file
		return 0;			// Early return  **
	}

	// Read out the format data
	if (mmioRead(hWaveFile, (char *)&formatdesc, sizeof(formatdesc))!=sizeof(formatdesc))
	{
		ErrorLogger::Write(L"Error in wave format of ");
		ErrorLogger::Writeln(filename);

		mmioClose(hWaveFile,0);
		return 0;			// Early return  **
	}

	// Check this is a pcm format (a standard wav format)
	if (formatdesc.wFormatTag!=WAVE_FORMAT_PCM)
	{
		ErrorLogger::Write(L"Error in wave format of ");
		ErrorLogger::Writeln(filename);

		mmioClose(hWaveFile,0);
		return 0;			// Early return  **
	}

	// Pop upstairs so we can then get down to data chunk
	if (mmioAscend(hWaveFile, &child, 0)!=MMSYSERR_NOERROR )
	{
		ErrorLogger::Write(L"Couldn't ascend to data chunk of ");
		ErrorLogger::Writeln(filename);

		mmioClose(hWaveFile,0);
		return 0;			// Early return  **
	}

	// Now drop into data chunk
	child.ckid=mmioFOURCC('d','a','t','a');

	if (mmioDescend(hWaveFile, &child,&parent, MMIO_FINDCHUNK)!=MMSYSERR_NOERROR)
	{
		ErrorLogger::Write(L"Couldn't find data section in wave file ");
		ErrorLogger::Writeln(filename);

		mmioClose(hWaveFile,0);			// Error - close the wave file
		return 0;			// Early return  **
	}


	// *************************************************************
	// Now that the info from the file has been stored, it is possible to
	// Create a sound buffer ready to hold the data, so it back to directX

	// Set the descriptor for the sound to be created.

	memset(&dsbd,0,sizeof(dsbd));
	dsbd.dwSize=sizeof(dsbd);
	dsbd.dwFlags = DSBCAPS_CTRLDEFAULT;				// Default features
	dsbd.dwBufferBytes=child.cksize;				// Set bytes needed to store
	dsbd.lpwfxFormat=&formatdesc;					// The format descriptor (got earlier from the file)

	HRESULT err = lpds->CreateSoundBuffer(&dsbd,&(temp.lpSoundBuffer),NULL);
	if (FAILED(err))
	{
		temp.lpSoundBuffer=nullptr;
		ErrorLogger::Writeln(L"Could not create a sound buffer");
		ErrorLogger::Writeln(MySoundEngine::ErrorString(err));
		return 0;			// Early return  **
	}

	// ************************************************************
	// The file is open, the buffer is created. Now to read all the data in.

	// Load data into a buffer
	tempBuffer = (UCHAR *)malloc(child.cksize);
	mmioRead(hWaveFile, (char*)tempBuffer, child.cksize);

	// Close the file
	mmioClose(hWaveFile,0);

	// Locking the Dsound buffer

	err = temp.lpSoundBuffer->Lock(0, child.cksize, (void**) &tempPtr1,
							&length1, (void**) &tempPtr2,
							&length2, DSBLOCK_FROMWRITECURSOR);
	if(FAILED(err))
	{
		ErrorLogger::Writeln(L"Couldn't lock the sound buffer.");
		ErrorLogger::Writeln(MySoundEngine::ErrorString(err));
		free(tempBuffer);
		temp.lpSoundBuffer->Release();
		temp.lpSoundBuffer=nullptr;
		return 0;			// Early return  **
	}

	// Copy the two bits of the buffer
	memcpy(tempPtr1, tempBuffer, length1);
	memcpy(tempPtr2, tempBuffer+length1, length2);

	// Unlock the Dsound buffer
	err = temp.lpSoundBuffer->Unlock(tempPtr1,
							length1, tempPtr2,
							length2);
	if(FAILED(err))
	{
		ErrorLogger::Writeln(L"Couldn't unlock the sound buffer.");
		ErrorLogger::Writeln(MySoundEngine::ErrorString(err));
		free(tempBuffer);
		temp.lpSoundBuffer->Release();
		temp.lpSoundBuffer=nullptr;
		return 0;			// Early return  **
	}

	free(tempBuffer);

	m_MySoundList.insert(std::pair<SoundIndex, MySound>(m_NextSoundIndex, temp));

	return m_NextSoundIndex++;
}

ErrorType MySoundEngine::Unload(SoundIndex sound)
{
	std::map<SoundIndex, MySound>::iterator it = m_MySoundList.find(sound);
	MySound& sb = it->second;

	if (sb.lpSoundBuffer)				// If lpSoundBuffer is not null
	{
		sb.lpSoundBuffer->Release();	// Attempt to release it

		sb.lpSoundBuffer=nullptr;

		m_MySoundList.erase(it);

		return SUCCESS;
	}
	return FAILURE;
}

ErrorType MySoundEngine::UnloadAllSounds()
{
	ErrorType answer = SUCCESS;
	std::map<SoundIndex, MySound>::iterator it = m_MySoundList.begin();
	for(;it!= m_MySoundList.end();it++ )
	{
		MySound& sb = it->second;

		if (sb.lpSoundBuffer)				// If lpSoundBuffer is not null
		{
			sb.lpSoundBuffer->Release();	// Attempt to release it

			sb.lpSoundBuffer=nullptr;
		}
	}

	// Erase all but the first one (this is a dummy sound to be returned
	// when attempting to find a nonexistent SoundIndex.
	it = m_MySoundList.begin();
	it++;
	m_MySoundList.erase(it, m_MySoundList.end());
	return answer;
}

ErrorType MySoundEngine::SetVolume(SoundIndex sound, int volume)
{
	MySound& sb = FindSound(sound);
	if(!sb.lpSoundBuffer)
	{
		ErrorLogger::Writeln(L"Sound not found.");
		return FAILURE;
	}
	HRESULT err = sb.lpSoundBuffer->SetVolume(volume);
	if (FAILED(err))
	{
		ErrorLogger::Write(L"Failed to set volume for a sound:");
		ErrorLogger::Writeln(sb.m_sourceFileName.c_str());
		ErrorLogger::Writeln(MySoundEngine::ErrorString(err));
		return FAILURE;
	}
	return SUCCESS;
}

ErrorType MySoundEngine::SetFrequency(SoundIndex sound, int frequency)
{
	MySound& sb = FindSound(sound);

	if(!sb.lpSoundBuffer)
	{
		ErrorLogger::Writeln(L"Sound not found in SetFrequency.");
		return FAILURE;
	}
	HRESULT err = sb.lpSoundBuffer->SetFrequency(frequency);
	if (FAILED(err))
	{
		ErrorLogger::Write(L"Failed to set frequency for a sound: ");
		ErrorLogger::Writeln(sb.m_sourceFileName.c_str());
		ErrorLogger::Writeln(MySoundEngine::ErrorString(err));
		return FAILURE;
	}
	return SUCCESS;
}

ErrorType MySoundEngine::SetPan(SoundIndex sound, int pan)
{
	MySound& sb = FindSound(sound);
	if(!sb.lpSoundBuffer)
	{
		ErrorLogger::Writeln(L"Sound buffer not created.");
		return FAILURE;
	}
	HRESULT err = sb.lpSoundBuffer->SetPan(pan);
	if (FAILED(err))
	{
		ErrorLogger::Write(L"Failed to pan a sound:");
		ErrorLogger::Writeln(sb.m_sourceFileName.c_str());
		ErrorLogger::Writeln(MySoundEngine::ErrorString(err));
		return FAILURE;
	}
	return SUCCESS;
}

ErrorType MySoundEngine::Play(SoundIndex sound, bool looping)
{
	// The first two numbers in the Play() functions below
	// are always zero. The third controls whether to loop,
	// or just play once.

	MySound& sb = FindSound(sound);

	if(!sb.lpSoundBuffer)
	{
		ErrorLogger::Writeln(L"Sound buffer not created.");
	}
	else
	{

		DWORD flag =0;
		if(looping)
		{
			flag = DSBPLAY_LOOPING;
		}
		HRESULT err = sb.lpSoundBuffer->Play(0,0, flag);
		if (FAILED(err))
		{
			ErrorLogger::Write(L"Failed to play a sound: ");
			ErrorLogger::Writeln(sb.m_sourceFileName.c_str());
			ErrorLogger::Writeln(MySoundEngine::ErrorString(err));
			return FAILURE;	
		}
		return SUCCESS;
	}	// if lpSoundBuffer not NULL
	return FAILURE;	
}

ErrorType MySoundEngine::Stop(SoundIndex sound)
{
	MySound& sb = FindSound(sound);
	if(!sb.lpSoundBuffer)
	{
		ErrorLogger::Writeln(L"Sound buffer not created.");
		return FAILURE;
	}

	HRESULT err = sb.lpSoundBuffer->Stop();
	if (FAILED(err))
	{
		ErrorLogger::Write(L"Failed to stop a sound: ");
		ErrorLogger::Writeln(sb.m_sourceFileName.c_str());
		ErrorLogger::Writeln(MySoundEngine::ErrorString(err));
		return FAILURE;
	}

	return SUCCESS;
}

