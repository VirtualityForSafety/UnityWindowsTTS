# UnityWindowsTTS
Text-To-Speech for Unity + Windows.
We thank to [Chad Weisshaar](https://chadweisshaar.com/blog/) for allowing to share this source code.
This source code was originally from [here](https://chadweisshaar.com/blog/2015/07/02/microsoft-speech-for-unity/). 


## Environment
* OS: Windows 10
* Visual studio: Microsoft Visual Studio Community 2017
* Unity: Unity3D 2019.1.7f1
* Voices: [Microsoft Speech API (SAPI) 5.3](https://docs.microsoft.com/en-us/previous-versions/windows/desktop/ms723627(v=vs.85))

## Requirements
* MS language pack: [https://support.microsoft.com/en-us/help/14236/language-packs](https://support.microsoft.com/en-us/help/14236/language-packs)
* MS TTS package: [https://support.office.com/en-us/article/how-to-download-text-to-speech-languages-for-windows-10-d5a6b612-b3ae-423f-afa5-4f6caf1ec5d3](https://support.office.com/en-us/article/how-to-download-text-to-speech-languages-for-windows-10-d5a6b612-b3ae-423f-afa5-4f6caf1ec5d3)

## How to try
* Open the solution file: WindowsTTS.sln
* Make sure the active build configuration is x64.
* Build! and then WindowsTTS.dll will be appeared in UnityWindowsTTS\Assets\Plugins.
* Open the sample scene file in UnityWindowsTTS\Assets\Scenes folder.
* Check the WindowsTTS.dll is in the right place.
* Run the scene and press the button.
* If it doesn't make any sound: 
  * Select the Narrator in the scene, press Add Component, and select 'Narrator'.
  * Select the Button under Canvas in the scene, drag and drop the Narrator to the object in On Click() at Button (Script), and change it from No function to Narrator.TestSpeech.
  * Then give it a try.

## Tip
* You can change the voice by 'Voice Idx' of Narrator, but the index seems to be assigned dynamic somehow. We will update that later.

## License
* MIT license

## Maintainer
* [Jinki Jung](https://github.com/JinkiJung)

