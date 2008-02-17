using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace GentagDemo
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [MTAThread]
        static void Main()
        {
            Start:
            try
            {
                Application.Run(new demoClient());
            }
            catch (Exception e)
            {
                goto Start;
            }
        }
    }
}