using System;


namespace ListItemNS
{
    /// <summary>
    /// Summary description for Class1
    /// </summary>
    //ListItem class
    public class ListItem
    {
        private string ShortDesc = "";
        private string rfidNum = "";
        private int imageIndex = -1;
        private int MissingAdded = 0;

        public ListItem(string rfid, string shortdesc, int b)
        {
            rfidNum = rfid;
            ShortDesc = shortdesc;
            MissingAdded = b;
        }

        public string shortDesc
        {
            get
            {
                return ShortDesc;
            }
            set
            {
                ShortDesc = value;
            }
        }

        public string RFIDNum
        {
            get
            {
                return rfidNum;
            }
            set
            {
                rfidNum = value;
            }
        }

        public int ImageIndex
        {
            get
            {
                return imageIndex;
            }
            set
            {
                imageIndex = value;
            }
        }

        public int missingAdded
        {
            get
            {
                return MissingAdded;
            }
            set
            {
                MissingAdded = value;
            }
        }
    }
}