using System;
using System.Diagnostics;
using System.Net;

namespace VoIP
{
    /// <summary>
    /// Contains a connection for a VoIP network
    /// </summary>
    public class VoIPConnection
    {
        private const String java_home = "C:\\Program Files (x86)\\Java\\jdk1.5.0_06\\bin\\";
        private const String mjuaDirectory = "lib\\mjua";
        private const String mjuaCommand = java_home + "java";
        private String sipRealm;
        private String sipUsername;
        private String sipPassword;
        private String sipRegTime;
        private String sipLocalPort;
        private String ipAddr;
        public VoIPConnection(String sipUsername, String sipPassword, String sipRealm, String sipLocalPort, String sipRegTime)
        {
            this.sipUsername = sipUsername;
            this.sipPassword = sipPassword;
            this.sipRealm = sipRealm;
            this.sipLocalPort = sipLocalPort;
            this.sipRegTime = sipRegTime;
            this.ipAddr = getIPAddr();
        }

        public void call(String sipDestination, String outputFile)
        {
            String javaArguments = "-cp lib\\sip.jar;lib\\ua.jar local.ua.CommandLineUA";
            String arguments = "-g " + sipRegTime + " -c " + sipDestination + "@" + sipRealm +
                " --username " + sipUsername + " --passwd " + sipPassword + " --realm " + sipRealm +
                " --via-addr " + ipAddr + " -p " + sipLocalPort + " --send-file " + outputFile;
            Process voipcall = new Process();
            voipcall.StartInfo.UseShellExecute = false;
            voipcall.StartInfo.RedirectStandardOutput = false;
            voipcall.StartInfo.RedirectStandardError = false;
            voipcall.StartInfo.CreateNoWindow = false;
            voipcall.StartInfo.FileName = mjuaCommand;
            voipcall.StartInfo.Arguments = javaArguments + " " + arguments;
            Console.Out.WriteLine(javaArguments + " " + arguments);
			//textBox1.Text += javaArguments + " " + arguments + "\n";
            voipcall.StartInfo.WorkingDirectory = mjuaDirectory;
            voipcall.Start();
            /*String output;
            while ((output = voipcall.StandardOutput.ReadLine()) != null)
            {
                System.Console.WriteLine(output);
                if (output.Equals("UA: ACCEPTED/CALL"))
                {
                    return;
                }
            }
             throw new VoIPException("call failed");*/
        }

        private String getIPAddr()
        {
            IPHostEntry ipfromhosts = Dns.GetHostByName(Dns.GetHostName());
            return ipfromhosts.AddressList.GetValue(0).ToString();
        }
    }
}
