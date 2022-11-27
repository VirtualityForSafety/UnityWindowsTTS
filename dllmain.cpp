// original contributor: Chad Weisshaar
// source code is originated from: https://chadweisshaar.com/blog/2015/07/02/microsoft-speech-for-unity/
// current contributor: Jinki Jung

#pragma warning(disable: 4996)

#include "pch.h"
#include <atlbase.h>
#include "WindowsTTS.h"

#include <sapi.h>
#include <sphelper.h>  
//#include <iostream>

namespace WindowsTTS {

	void changeVoiceByIndex(CComPtr<ISpObjectToken> cpVoiceToken, CComPtr<IEnumSpObjectTokens> cpEnum, CComPtr<ISpVoice> cpVoice, ULONG ulCount) {

		if (targetVoiceIndex != currentVoiceIndex) {
			// Enumerate the available voices.
			HRESULT hr = SpEnumTokens(SPCAT_VOICES, NULL, NULL, &cpEnum);

			if (SUCCEEDED(hr))
			{
				// Get the number of voices.
				hr = cpEnum->GetCount(&ulCount);
			}

			targetVoiceIndex = targetVoiceIndex % ulCount;
			currentVoiceIndex = ulCount;
			// Obtain a list of available voice tokens, set
			// the voice to the token, and call Speak.
			while (SUCCEEDED(hr) && targetVoiceIndex != currentVoiceIndex)
			{
				cpVoiceToken.Release();

				if (SUCCEEDED(hr))
				{
					hr = cpEnum->Next(1, &cpVoiceToken, NULL);
				}

				if (SUCCEEDED(hr))
				{
					hr = cpVoice->SetVoice(cpVoiceToken);
				}

				if (SUCCEEDED(hr))
				{

				}
				currentVoiceIndex = (currentVoiceIndex - 1) % ulCount;
			}

			//hr = cpVoice->Speak(L"Alright. Let's get started!", SPF_DEFAULT, NULL);
		}
	}

	const wchar_t *GetWC(const char *c)
	{
		const size_t cSize = strlen(c) + 1;
		wchar_t* wc = new wchar_t[cSize];
		mbstowcs(wc, c, cSize);

		return wc;
	}

	void changeVoiceByCondition(wchar_t* szRequiredAttribs, CComPtr<ISpVoice> cpVoice) {
		if (szRequiredAttribs != nullptr) {
			szRequiredAttribs = L"gender=male";
			ISpObjectToken* cpToken(NULL);
			SpFindBestToken(SPCAT_VOICES, szRequiredAttribs, L"", &cpToken);
			cpVoice->SetVoice(cpToken);
			cpToken->Release();
			szRequiredAttribs = nullptr;
		}
	}

  void speechThreadFunc()
  {
	  // Declare local identifiers:
	  HRESULT                        hr = S_OK;
	  CComPtr<ISpObjectToken>        cpVoiceToken;
	  CComPtr<IEnumSpObjectTokens>   cpEnum;
	  CComPtr<ISpVoice>              cpVoice;
	  ULONG                          ulCount = 0;
	  speaking = false;
	  condition = nullptr;

	  if (FAILED(::CoInitializeEx(NULL, COINITBASE_MULTITHREADED)))
	  {
		  theStatusMessage = L"Failed to initialize COM for Voice.";
		  return;
	  }

	  // Create the SAPI voice.
	  hr = cpVoice.CoCreateInstance(CLSID_SpVoice);

	  if (!SUCCEEDED(hr))
	  {
		  LPSTR pText = 0;

		  ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			  NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), pText, 0, NULL);
		  LocalFree(pText);
		  theStatusMessage = L"Failed to create Voice instance.";
		  return;
	  }

    //ISpVoice * pVoice = NULL;

    //HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);
    
    theStatusMessage = L"Speech ready.";

    SPVOICESTATUS voiceStatus;
    wchar_t* priorText = nullptr;
    while (!shouldTerminate)
    {
		changeVoiceByIndex(cpVoiceToken, cpEnum, cpVoice, ulCount);
		changeVoiceByCondition(condition, cpVoice);
		
      //pVoice->GetStatus(&voiceStatus, NULL);
	cpVoice->GetStatus(&voiceStatus, NULL);
      if (voiceStatus.dwRunningState == SPRS_IS_SPEAKING)
      {
		speaking = true;
        if (priorText == nullptr)
          theStatusMessage = L"Error: SPRS_IS_SPEAKING but text is NULL";
        else
        {
          theStatusMessage = L"Speaking: ";
          theStatusMessage.append(priorText);
          if (!theSpeechQueue.empty())
          {
            theMutex.lock();
            if (!theSpeechQueue.empty())
            {
                if (lstrcmpW(theSpeechQueue.front(), priorText) == 0)
                {
                    delete[] theSpeechQueue.front();
                    theSpeechQueue.pop_front();
                }
            }
            theMutex.unlock();
          }
        }
      }
      else
      {
		speaking = false;
        theStatusMessage = L"Waiting.";
        if (priorText != NULL)
        {
          delete[] priorText;
          priorText = NULL;
        }
        if (!theSpeechQueue.empty())
        {
          theMutex.lock();
          if (!theSpeechQueue.empty())
          {
              priorText = theSpeechQueue.front();
              theSpeechQueue.pop_front();
          }
          theMutex.unlock();
          //pVoice->Speak(priorText, SPF_IS_XML | SPF_ASYNC, NULL);
		  cpVoice->Speak(priorText, SPF_IS_XML | SPF_ASYNC, NULL);
        }
      }
      Sleep(50);
    }
    //pVoice->Pause();
    //pVoice->Release();
	cpVoice->Pause();
	//cpVoice->Release();

    theStatusMessage = L"Speech thread terminated.";
  }

  bool isSpeaking() {
	  return speaking;
  }

  void addToSpeechQueue(const char* text)
  {
    if (text)
    {
      int len = strlen(text) + 1;
      wchar_t *wText = new wchar_t[len];

      memset(wText, 0, len);
      ::MultiByteToWideChar(CP_UTF8, NULL, text, -1, wText, len);

      theMutex.lock();
      theSpeechQueue.push_back(wText);
      theMutex.unlock();
    }
  }

  void clearSpeechQueue()
  {
    theMutex.lock();
    theSpeechQueue.clear();
    theMutex.unlock();
  }
  void changeVoice(int vIdx) {
	  targetVoiceIndex = vIdx;
  }
  void initSpeech()
  {
    shouldTerminate = false;
    if (theSpeechThread != nullptr)
    {
      theStatusMessage = L"Windows Voice thread already started.";
      return;
    }
    theStatusMessage = L"Starting Windows Voice.";
    theSpeechThread = new std::thread(WindowsTTS::speechThreadFunc);
  }
  void destroySpeech()
  {
    if (theSpeechThread == nullptr)
    {
      theStatusMessage = L"Speach thread already destroyed or not started.";
      return;
    }
    theStatusMessage = L"Destroying speech.";
    shouldTerminate = true;
    theSpeechThread->join();
    theSpeechQueue.clear();
    delete theSpeechThread;
    theSpeechThread = nullptr;
    CoUninitialize();
    theStatusMessage = L"Speech destroyed.";
  }
  void statusMessage(char* msg, int msgLen)
  {
    size_t count;
    wcstombs_s(&count, msg, msgLen, theStatusMessage.c_str(), msgLen);
  }
}


BOOL APIENTRY DllMain(HMODULE, DWORD ul_reason_for_call, LPVOID)
{
  switch (ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH:
    break;
  }
  
  return TRUE;
}