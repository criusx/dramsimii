using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Protocol;
using System.Net.Sockets;
using System.Threading;

namespace GenTag_Server
{
    public partial class mainForm : Form
    {
        private daemon d;

        private bool isRunning = true;

        private ArrayList messages = new ArrayList();

        private Object messagewriter = new Object();

        public mainForm()
        {
            InitializeComponent();

            (new Thread(new ThreadStart(Startup))).Start();
        }

        private void Startup()
        {
            try
            {
                d = new daemon(new AsyncCallback(acceptClient), Protocol.Packet.port);

                writeLog("Daemon listening");

                while (isRunning)
                    Thread.Sleep(1000);
            }
            catch (SocketException ex)
            {
                writeLog(ex.Message);
            }

            d.Stop();
        }

        private void acceptClient(IAsyncResult result)
        {
            TcpClient client = ((daemon)result.AsyncState).EndAcceptTcpClient(result);
            ((daemon)result.AsyncState).BeginAcceptTcpClient(new AsyncCallback(acceptClient), (daemon)result.AsyncState);

            Thread thr = new Thread(new ParameterizedThreadStart(serviceClient));
            thr.Start(client);
        }

        private void serviceClient(object clientConnection)
        {
            TcpClient client = (TcpClient)clientConnection;

            writeLog("New Client " + client.ToString());

            try
            {
                while (isRunning && client.Connected)
                {
                    while (!client.GetStream().DataAvailable)
                    {
                        Thread.Sleep(50);
                    }

                    Packet newPacket = new Packet();

                    newPacket.Stream = client.GetStream();

                    newPacket.GetPacket();

                    switch (newPacket.Type)
                    {
                        case PacketTypes.UnknownType:
                            continue;

                        case PacketTypes.DescriptionRequest:
                            writeLog(newPacket.ToString());
                            Packet response = new Packet(PacketTypes.DescriptionResponse);
                            response.Data = System.Text.Encoding.ASCII.GetBytes("None so far");
                            response.Stream = client.GetStream();
                            response.SendPacket();
                            break;

                        case PacketTypes.CloseConnectionRequest:
                            client.Close();
                            break;
                    }
                }
            }
            catch (SocketException ex)
            {
                writeLog(ex.Message);
            }
        }

        private void mainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            isRunning = false;
        }

        private void writeLog(string str)
        {
            lock (messagewriter)
            {
                messages.Add(str);
                timer1_Tick(this, new EventArgs());
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (listBox1.InvokeRequired)
            {
                listBox1.BeginInvoke(new EventHandler(timer1_Tick), new object[] { sender, e });
            }
            else
            {
                if (messages.Count == 0)
                    return;

                lock (messagewriter)
                {                
                    listBox1.BeginUpdate();

                    while (listBox1.Items.Count >= logLimitSize.Value)
                        listBox1.Items.RemoveAt(0);
                    for (int i = 0; i < messages.Count; i++)
                        listBox1.Items.Add(@"[" + DateTime.Now.ToString() + @"] " + messages[i]);

                    listBox1.SelectedIndex = listBox1.Items.Count - 1;

                    listBox1.EndUpdate();

                    messages.Clear();
                }
            }
        }
    }
}