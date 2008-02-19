using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Reflection;
using System.Diagnostics;

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
            authenticationWS.AuthenticationWebService ws = new authenticationWS.AuthenticationWebService();
            ws.Timeout = 10000;
                        
            try
            {
                Application.Run(new demoClient());
            }
            catch (Exception e)
            {
                if (DialogResult.Yes == MessageBox.Show("The application has encountered an error and must restart, click Yes to report this error and restart, No to quit.", "Error", MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation, MessageBoxDefaultButton.Button1))
                {
                    for (int i = 5; i > 0; i--)
                    {
                        try
                        {
                            ws.sendError(e.ToString(), e.StackTrace, e.Message);
                            break;
                        }
                        catch (Exception)
                        {
                        }
                    }
                    //demoClient.Reset();
                    //goto Start;
                    
                    //System.Reflection.AssemblyName an = System.Reflection.Assembly.GetExecutingAssembly().GetName().CodeBase;
                    Application.Exit();
                    AppDomain.CreateDomain("abc").ExecuteAssembly(Assembly.GetExecutingAssembly().GetName().CodeBase);
                    //ProcessStartInfo process = new ProcessStartInfo(Assembly.GetExecutingAssembly().GetName().CodeBase, "");
                    //process.UseShellExecute = true;
                    //Process.Start(process);
                    

                }
                else
                {
                    Application.Exit();
                }
            }
        }
    }
}