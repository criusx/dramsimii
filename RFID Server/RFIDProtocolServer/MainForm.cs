using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Threading;
using System.Windows.Forms;

using RFIDProtocolLib;

namespace RFIDProtocolServer
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        Thread t;
        Daemon d;
        bool isDone;

        private void MainForm_Load(object sender, EventArgs e)
        {
            t = new Thread(new ThreadStart(RunDaemon));
            isDone = false;
            t.Start();
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            isDone = true;
            //d.Stop();
        }

        private void RunDaemon()
        {
            d = new Daemon(Daemon.PORT);
            d.Start();

            while (!isDone)
            {
                Console.Out.WriteLine("Waiting for connection.");
                ServerConnection s = d.AcceptClientConnection();
                Console.Out.WriteLine("Got a connection, waiting for connect packet");
                s.WaitForConnectPacket();
                Console.Out.WriteLine("Got connect packet, sending response");
                s.SendConnectResponsePacket();

                while (!isDone)
                {
                    Console.Out.WriteLine("waiting for query");
                    QueryRequest request = s.WaitForQueryPacket();
                    Console.Out.WriteLine("Got request for {0}, sending response", request.Rfid.ToString());
                    QueryResponse response = DBBackend.QueryRFID(request);
                    s.SendQueryResponsePacket(response);
                }
            }
        }
    }
}