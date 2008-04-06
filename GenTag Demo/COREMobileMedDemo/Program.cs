using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Reflection;

namespace COREMobileMedDemo
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [MTAThread]
        static void Main()
        {
            try
            {
                Application.Run(new mobileMedDemo());
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
                        catch (Exception)
                        {
                        }
                    }
                    Cursor.Current = Cursors.WaitCursor;
                    Application.Exit();

                    AppDomain.CreateDomain("abc").ExecuteAssembly(Assembly.GetExecutingAssembly().GetName().CodeBase);
                    Cursor.Current = Cursors.Default;
                    
                }
                else
                {
                    Application.Exit();
                    throw;
                }
            }

        }
    }
}