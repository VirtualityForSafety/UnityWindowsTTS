using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Text;
using System.Runtime.InteropServices;
using System.IO;

namespace ACTA
{
    public class Narrator : MonoBehaviour
    {

        public enum CONTEXT
        { TUTORIAL, TRAINING_ANN, TRAINING_TUT, ASSESSMENT };

        [DllImport("WindowsTTS")]
        public static extern void initSpeech();

        [DllImport("WindowsTTS")]
        public static extern void destroySpeech();

        [DllImport("WindowsTTS")]
        public static extern void addToSpeechQueue(byte[] s);
        //[DllImport("WindowsVoice", CharSet=CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        //public static extern void addToSpeechQueue([MarshalAs(UnmanagedType.LPStr)] string s);

        [DllImport("WindowsTTS")]
        public static extern void clearSpeechQueue();

        [DllImport("WindowsTTS")]
        public static extern void statusMessage(StringBuilder str, int length);

        [DllImport("WindowsTTS")]
        public static extern void changeVoice(int vIdx);
        
        public CONTEXT mode;

        public static Narrator theVoice = null;

        public int voiceIdx = 0;

        static List<string> keyValue = new List<string>();

        static List<string> keyValueOnTask = new List<string>();
        static int currIdx = 0;

        void OnEnable()
        {
            if (theVoice == null)
            {
                theVoice = this;
                Debug.Log("Initializing speech");
                initSpeech();
            }
        }

        void OnDestroy()
        {
            if (theVoice == this)
            {
                Debug.Log("Destroying speech");
                destroySpeech();
                theVoice = null;
            }
        }

        public static void speak(string msg, bool interruptable = false)
        {
            Encoding encoding = System.Text.Encoding.GetEncoding(System.Threading.Thread.CurrentThread.CurrentCulture.TextInfo.ANSICodePage);
            var data = encoding.GetBytes(msg);
            if (interruptable)
                clearSpeechQueue();
            addToSpeechQueue(data);
            //addToSpeechQueue(msg);
        }

        private void Awake()
        {
            changeVoice(voiceIdx);
        }

        public void TestSpeech()
        {
            Narrator.speak("Do you hear me?", false);
        }

        private void Update()
        {
            //FOR TESTING
            /*
            if (Input.GetKeyDown(KeyCode.Space))
            {
                Narrator.speak("Is there any problem in there?", false);
                //Narrator.speak("거기 문제 있나요?", false);
            }
            //*/
        }

        private void OnApplicationQuit()
        {
            Narrator.destroySpeech();
        }


    }

}
