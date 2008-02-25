using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Reflection;
using System.Diagnostics;
using System.Threading;

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
            //int a, b;
            //System.Threading.ThreadPool.GetMaxThreads(out a,out b);
            //MessageBox.Show("has " + a.ToString() + " threads.\nhas " + b.ToString() + " IO threads.");
            System.Threading.ThreadPool.SetMaxThreads(50, 500);
            
            try
            {
                Application.Run(new demoClient());
            }
            catch (Exception e)
            {
                if (DialogResult.Yes == MessageBox.Show("The application has encountered an error and must restart, click Yes to report this error and restart, No to quit.", "Error", MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation, MessageBoxDefaultButton.Button1))
                {
                    authenticationWS.AuthenticationWebService ws = new authenticationWS.AuthenticationWebService();
                    ws.Timeout = 10000;

                    for (int i = 5; i > 0; i--)
                    {
                        try
                        {
                            ws.sendError(e.ToString(), e.StackTrace, e.Message);
                            break;
                        }
                        catch (Exception ex)
                        {
                        }
                    }
                    //demoClient.Reset();
                    //goto Start;
                    
                    //System.Reflection.AssemblyName an = System.Reflection.Assembly.GetExecutingAssembly().GetName().CodeBase;
                    Cursor.Current = Cursors.WaitCursor;
                    Application.Exit();

                    AppDomain.CreateDomain("abc").ExecuteAssembly(Assembly.GetExecutingAssembly().GetName().CodeBase);
                    Cursor.Current = Cursors.Default;
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