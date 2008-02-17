using System;
using System.Linq;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace WDReader
{
    #region EventArgs
    public class ReaderErrorEventArgs : EventArgs
    {
        private string errorReason;

        public ReaderErrorEventArgs(string value)
        {
            errorReason = value;
        }

        public string Reason
        {
            get
            {
                return errorReason;
            }
        }
    }

    public class TagFoundEventArgs : EventArgs
    {
        private string tagID;

        private bool doneReading;

        public TagFoundEventArgs(string _tag, bool _done)
        {
            tagID = _tag;
            doneReading = _done;
        }

        public string Tag
        {
            get
            {
                return tagID;
            }
        }

        public bool Done
        {
            get
            {
                return doneReading;
            }
        }
    }
#endregion

    public class WDReader
    {
#region Members
        private System.Threading.Timer tagCheckTimer;
        private bool reinitializationNeeded;
        private TimerCallback timerCallback;
        private Mutex functionLock;
        private SDiD1020.Utility.WDIUtility wdiUtility;
        private SDiD1020.ISO15693.ISO15693Card card;
#endregion
        #region Events
        public event EventHandler<TagFoundEventArgs> TagReceived;
        public event EventHandler<ReaderErrorEventArgs> ReaderError;
        #endregion

        public WDReader()
        {
            timerCallback = new TimerCallback((object)tagCheckTimer_Tick);
            reinitializationNeeded = true;
            wdiUtility = new SDiD1020.Utility.WDIUtility();
            card = new SDiD1020.ISO15693.ISO15693Card();
        }        

        public bool Running
        {
            set
            {
                if (value == true)
                    tagCheckTimer = new Timer(timerCallback, null, 50, 250);
                else
                    tagCheckTimer.Dispose();
            }
        }

        enum ERR
        {
            NONE = 0,
            CARD_AVAILABLE = 1,
            EJECTED = 150102,
        }

        // reentrant worker function called periodically by the timer
        private void tagCheckTimer_Tick()
        {
            // if a previous thread is doing a read, just skip this time
            if (functionLock.WaitOne(0, true))
            {
                //////////////////////////////////////////////
                if (reinitializationNeeded)
                {
                    // change protocol to 15693
                    wdiUtility.ChangeProtocol(2, 1);
                    reinitializationNeeded = false;
                }

                //ERR ec = (ERR)card.IsCardAvailable();

                switch ((ERR)card.IsCardAvailable())
                {
                    case ERR.NONE:
                        //No card available do nothing
                        if (status.Text != "Searching...")
                        {
                            status.Text = "Searching...";
                            status.Update();
                        }
                        break;

                    case ERR.CARD_AVAILABLE:
                        //Card available
                        byte[] uid = new byte[ICODE_SLI_UID_SIZE];
                        card.GetUID(uid);

                        DisplayUID();
                        if (IsNewUID(uid))
                        {
                            StoreUID(uid);
                            ReadDataFromCard();
                        }
                        break;

                    case ERR.EJECTED:
                        //SDiD in not inserted
                        if (status.Text != "Please insert SDiD.")
                        {
                            status.Text = "Please insert SDiD";
                            status.Update();
                        }
                        break;

                    default:
                        //Some other error
                        //Ignore for now
                        break;
                }
                //////////////////////////////////////////////
                functionLock.ReleaseMutex();
            }
        }
    }
}
