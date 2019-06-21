// original contributor: Chad Weisshaar
// source code is originated from: https://chadweisshaar.com/blog/2015/07/02/microsoft-speech-for-unity/
// current contributor: Jinki Jung

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#include <mutex>
#include <list>
#include <thread>

namespace WindowsTTS {
  extern "C" {
    DLL_API void __cdecl initSpeech();
    DLL_API void __cdecl addToSpeechQueue(const char* text);
    DLL_API void __cdecl clearSpeechQueue();
    DLL_API void __cdecl destroySpeech();
	DLL_API void __cdecl changeVoice(int vIdx);
	DLL_API bool __cdecl isSpeaking();
    DLL_API void __cdecl statusMessage(char* msg, int msgLen);
  }

  ULONG currentVoiceIndex = 0;
  ULONG targetVoiceIndex = 0;
  bool speaking = false;
  std::mutex theMutex;
  std::list<wchar_t*> theSpeechQueue;
  std::thread* theSpeechThread = nullptr;
  bool shouldTerminate = false;
  wchar_t* condition;
  std::wstring theStatusMessage;
}