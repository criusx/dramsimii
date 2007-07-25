using System;
using System.Collections.Generic;
using System.Text;

namespace COREMobileMedDemo
{
    public class patientVitals
    {
        // the RFID Number
        private string RFIDNum;
        // the arrays of vital statistics
        private float[] temperatures;
    
        public patientVitals(string _RFIDNum, float[] _temperatures)
        {
            RFIDNum = new string(_RFIDNum.ToCharArray());
            temperatures = new float[_temperatures.Length];
            for (int i = 0; i < _temperatures.Length; i++)
                temperatures[i] = _temperatures[i];
        }

        public string RFID
        {
            get
            {
                return RFIDNum;
            }
        }

        public float[] temps
        {
            get
            {
                return temperatures;
            }
        }
    }
}
