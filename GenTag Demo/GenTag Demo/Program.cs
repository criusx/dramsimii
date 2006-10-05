using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace GenTag_Demo
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [MTAThread]
        static void Main()
        {
            Application.Run(new mainForm());
        }
    }
}