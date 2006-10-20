using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using System.Net.Sockets;
using System.IO;
using Oracle.DataAccess.Client;
using Oracle.DataAccess.Types;
using RFIDProtocolLibrary;
using ListItemNS;
using System.Xml.Serialization;
using System.Web.Services.Protocols;

namespace RFIDServer
{
    public partial class MainForm : Form
    {
        
        Daemon d;
        bool isRunning;

        private ArrayList messages;
        private Mutex messagesLock;

        private const string DBSERVER = "SRL";
        private const string DATABASE = "RFID";
        private Object messagewriter = new Object();

        public MainForm()
        {
            
            InitializeComponent();
            messages = new ArrayList();
            messagesLock = new Mutex();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            isRunning = true;

            this.Text = Text + " v" + System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString();
            Thread t = new Thread(new ThreadStart(RunDaemon));
            t.Start();
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            isRunning = false;
        }

        private void RunDaemon()
        {
            try
            {
                d = new Daemon(RFIDProtocolLibrary.Packet.port);
                writeLog("Starting Daemon...");
                d.Start();
                d.BeginAcceptClientConnection(new AsyncCallback(AcceptClient), d);
                writeLog("Daemon listening...");

                while (isRunning)
                {
                    Thread.Sleep(1000);
                }
            }
            catch (SocketException ex)
            {
                writeLog(ex.Message);
            }
            Console.Out.WriteLine("Closing Daemon...");

            d.Stop();
        }

        private void AcceptClient(IAsyncResult result)
        {
            writeLog("Received new client");

            Daemon d = (Daemon)result.AsyncState;
            ServerConnection s = d.EndAcceptClientConnection(result);

            Thread t = new Thread(new ParameterizedThreadStart(RunServer));
            t.Start(s);

            d.BeginAcceptClientConnection(new AsyncCallback(AcceptClient), d);
        }

        private void RunServer(object obj)
        {
            writeLog("Client thread running");

            ServerConnection s = (ServerConnection)obj;

            string MyConString = "User Id=" + usernameBox.Text + ";" +
                "Password=" + passwordBox.Text + ";" +
                "Data Source=" + dataSourceBox.Text;

            ArrayList inventoryTags = new ArrayList();

            try
            {
                while (isRunning && s.Connected)
                {
                    while (!s.GetStream().DataAvailable)
                    {
                        Thread.Sleep(5);
                    }

                    TLV newPacket = new TLV();

                    newPacket.ReadFromStream(s.GetStream());

                    if (newPacket.Type == 0)
                        continue;

                    switch (newPacket.Type)
                    {
                        #region QueryRequest
                        case (ushort)Packets.QueryRequest:
                            {
                                try
                                {
                                    QueryRequest req = new QueryRequest(newPacket.Value);

                                    req = addTag(ref inventoryTags, req);
                                }
                                catch (ArgumentException ex)
                                { }
                            } break;
                        #endregion

                        #region ReconcileFinishedHandler
                        case (ushort)Packets.ReconcileFinished:
                            {
                                ReconcileFinished req = new ReconcileFinished(newPacket.Value);

                                OracleConnection c = null;
                                OracleCommand cmd = null;
                                OracleGlobalization info = OracleGlobalization.GetClientInfo();
                                info.DateFormat = "MM/DD/YY HH:MI:SS AM";
                                OracleGlobalization.SetThreadInfo(info);

                                try
                                {
                                    // now go do requests
                                    c = new OracleConnection(MyConString);
                                    c.Open();

                                    cmd = new OracleCommand();
                                    cmd.Connection = c;

                                    cmd.CommandText = "InsertAlerts";

                                    cmd.CommandType = CommandType.StoredProcedure;

                                    OracleParameter prm1 = cmd.Parameters.Add("in_containerrfid", OracleDbType.Varchar2, req.containerRFID, ParameterDirection.Input);

                                    OracleParameter prm2 = cmd.Parameters.Add("in_alertdt", OracleDbType.Date, new OracleDate(req.dateTime), ParameterDirection.Input);

                                    OracleParameter prm3 = cmd.Parameters.Add("in_numbertocall", OracleDbType.Varchar2, req.numberToCall, ParameterDirection.Input);

                                    writeLog(cmd.CommandText + "," + req.containerRFID + "," + req.dateTime + "," + req.numberToCall);
                                    writeLog(cmd.CommandText + "," + prm1.Value + "," + prm2.Value + "," + prm3.Value);

                                    cmd.ExecuteNonQuery();
                                }
                                catch (OracleException e)
                                {
                                    writeLog(e.Message);
                                }
                                finally
                                {
                                    if (cmd != null)
                                        cmd.Dispose();
                                    if (c != null && c.State == ConnectionState.Open)
                                        c.Close();
                                    c.Dispose();
                                }
                            } break;
                        #endregion

                        #region RaiseAlertHandler
                        case (ushort)Packets.RaiseAlert:
                            {
                                // now go do requests
                                //OracleConnection c = new OracleConnection(MyConString);
                                //c.Open();

                                //OracleCommand cmd = new OracleCommand();
                                //cmd.Connection = c;

                                //string currentTime = DateTime.Now.ToString();

                                //cmd.CommandText = "INSERT INTO alerts VALUES(alertnum_seq.nextval" +
                                //    ",'" + "313233343536000000000046" +
                                //    "','" + PhoneNumber.ToString() +
                                //    "',2,0,'root@localhost','Radiation alert.'," + "'313233343536000000000046'" +
                                //    "TO_DATE('" + currentTime + "','MM/DD/YY HH:MI:SS AM'))" + ")";
                                //System.Console.WriteLine(cmd.CommandText);
                                //cmd.ExecuteNonQuery();

                                //writeLog(ref cmd);

                                //cmd.ExecuteNonQuery();

                                //c.Close();
                                //c.Dispose();
                                //VoIP.VoIPConnection voip = new VoIP.VoIPConnection("3796001", "bello", "inphonex.com", port.ToString(), "300");
                                //voip.call(PhoneNumber, "temp1.raw");
                                //port++;
                            } break;

                        #endregion

                        #region InfoHandler
                        case (ushort)Packets.Info: //Info Type, also signals the end of the tag stream
                            {
                                Info iP = new Info(newPacket.Value);

                                checkinItems(s, MyConString, ref inventoryTags, iP);
                            } break;
                        #endregion

                        #region CloseConnection
                        case (ushort)Packets.CloseConnection: // client decides when the connection is done
                            s.Close();
                            break;
                        #endregion

                        #region addRemoveItemHandler
                        case (ushort)Packets.addRemoveItem:

                            addRemoveItem aRI = new addRemoveItem(newPacket.Value);
                            OracleConnection con = null;
                            OracleCommand orCmd = null;

                            try
                            {

                                // now go do requests
                                con = new OracleConnection(MyConString);
                                con.Open();

                                orCmd = new OracleCommand();
                                orCmd.Connection = con;

                                if (aRI.remove == true)
                                {

                                    // for normal items
                                    orCmd.CommandText =
                                        "UPDATE scans " +
                                        "SET added_removed = 'REMOVED' " +
                                        "WHERE rfid=" + aRI.hostID + " " +
                                        "AND scantime=to_date('" + aRI.dateTime +
                                        "','MM/DD/YY HH:MI:SS AM') " +
                                        "AND manifest_num=" + aRI.manifestNum;
                                    writeLog(ref orCmd);
                                    orCmd.ExecuteNonQuery();
                                }
                                else
                                {
                                    // for missing items
                                    orCmd.CommandText =
                                            "UPDATE scans " +
                                            "SET added_removed = 'ADDED' " +
                                            "WHERE rfid=" + aRI.hostID + " " +
                                            "AND scantime=to_date('" + aRI.dateTime +
                                            "','MM/DD/YY HH:MI:SS AM') " +
                                            "AND manifest_num=" + aRI.manifestNum;
                                    writeLog(ref orCmd);
                                    orCmd.ExecuteNonQuery();
                                }

                            }
                            catch (Exception e)
                            {
                                writeLog(e.Message);
                            }
                            finally
                            {
                                if (orCmd != null)
                                    orCmd.Dispose();
                                if (con != null && con.State == ConnectionState.Open)
                                    con.Close();
                                con.Dispose();
                            }
                            break;
                        #endregion

                        default:
                            throw new Exception("Bad protocol type");
                    }
                }
            }
            catch (Exception e)
            {
                writeLog(e.Message.ToString());
            }
            writeLog("Client closed");
        }

        private void checkinItems(ServerConnection s, string MyConString, ref ArrayList inventoryTags, Info iP)
        {
            OracleConnection c = null;
            OracleTransaction Txn = null;
            OracleCommand cmd = null;
            OracleGlobalization info = OracleGlobalization.GetClientInfo();
            info.DateFormat = "MM/DD/YY HH:MI:SS AM";
            OracleGlobalization.SetThreadInfo(info);

            try
            {
                // now go do requests
                c = new OracleConnection(MyConString);
                c.Open();
                Txn = c.BeginTransaction(IsolationLevel.Serializable);

                //////////////////////////////////////////////////////////////////////////
                //cmd = new OracleCommand();
                //cmd.Connection = c;

                //string[] in_rfidd = new string[20];
                //int[] bindsize = new int[20];
                //cmd.CommandText = @"TESTPACKAGE.testing";
                //cmd.CommandType = CommandType.StoredProcedure;
                //for (int i = 0; i < 20; i++)
                //{
                //    in_rfidd[i] = @"0123456789abcdefghijklmn";
                //    bindsize[i] = 24;
                //}
                //OracleParameter a = cmd.Parameters.Add("in_rfid", OracleDbType.Varchar2, 20,  in_rfidd, ParameterDirection.Input);
                //a.CollectionType = OracleCollectionType.PLSQLAssociativeArray;
                //a.ArrayBindSize = bindsize;

                //cmd.ExecuteNonQuery();

                //////////////////////////////////////////////////////////////////////////
                cmd = new OracleCommand();
                cmd.Connection = c;

                OracleDate newScantime = new OracleDate(iP.dateTime);

                // determine the container's RFID
                cmd.CommandType = CommandType.Text;

                cmd.CommandText =
                    "SELECT rfid " +
                    "FROM descriptions " +
                    "WHERE shortdesc = 'Shipping Container 42D' AND (";

                for (int j = 0; j < inventoryTags.Count; j++)
                {
                    cmd.CommandText += "rfid='" + inventoryTags[j].ToString();

                    if (j < inventoryTags.Count - 1)
                        cmd.CommandText += "' OR ";
                    else
                        cmd.CommandText += "')";
                }

                writeLog(ref cmd);

                OracleDataReader reader = cmd.ExecuteReader();

                if (!reader.Read())
                {
                    writeLog("No rows found.");
                    s.SendQueryResponsePacket(new QueryResponse("No container RFID found.", -2, " ", 0, " "));
                    return;
                }

                string containerRFID = reader.GetString(0);

                reader.Close();

                writeLog("Container is: " + containerRFID);


                if (iP.isScan == 0) // generate a new manifest entry and prepare to retrieve that value
                {
                    // create a new manifestnum for the parent
                    cmd.CommandText =
                        "INSERT INTO manifest " +
                        "VALUES( manifestnum_seq.nextval,'" +
                        containerRFID + "',TO_DATE('" +
                        iP.dateTime + "','MM/DD/YY HH:MI:SS AM'))";

                    writeLog(ref cmd);

                    cmd.ExecuteNonQuery();

                    // get the manifest number, to be used in retrieving the whole manifest later
                    cmd.CommandText =
                        "SELECT manifestnum_seq.currval " +
                        "FROM dual";
                }
                else // then it is a scan and does not create a manifest, just find the manifest number
                {
                    // get manifest number based on container RFID
                    cmd.CommandText =
                        "SELECT max(manifest_num) " +
                        "FROM manifest " +
                        "WHERE rfid = '" + containerRFID + "'";
                }

                writeLog(ref cmd);

                reader = cmd.ExecuteReader();

                if (!reader.Read())
                {
                    writeLog("Subsequent scan without container tag");
                    //return;
                }

                decimal manifestNum = reader.GetDecimal(0);

                reader.Close();

                // do the inserts
                decimal[] in_manifestnum = new decimal[inventoryTags.Count];
                string[] in_rfid = new string[inventoryTags.Count];
                OracleDate[] in_scantime = new OracleDate[inventoryTags.Count];
                string[] in_latitude = new string[inventoryTags.Count];
                string[] in_longitude = new string[inventoryTags.Count];
                float[] in_temperature = new float[inventoryTags.Count];
                decimal[] in_elevation = new decimal[inventoryTags.Count];
                float[] in_relativehumidity = new float[inventoryTags.Count];
                char[] initial = new char[inventoryTags.Count];
                string[] in_containerrfid = new string[inventoryTags.Count];

                for (int i = 0; i < inventoryTags.Count; i++)
                {
                    in_manifestnum[i] = manifestNum;
                    in_rfid[i] = inventoryTags[i].ToString();
                    in_scantime[i] = newScantime;
                    in_latitude[i] = iP.Latitude;
                    in_longitude[i] = iP.Longitude;
                    in_temperature[i] = 0;
                    in_elevation[i] = 0;
                    in_relativehumidity[i] = 0;
                    initial[i] = iP.isScan == 0 ? 'Y' : 'N';
                    in_containerrfid[i] = containerRFID;
                }

                cmd.BindByName = true;

                cmd.CommandType = CommandType.StoredProcedure;

                cmd.ArrayBindCount = inventoryTags.Count;

                // run InsertScans
                cmd.CommandText =
                    "InsertScans";

                writeLog(cmd.CommandText);

                //OracleParameter prm1 = cmd.Parameters.Add("in_manifestnum", OracleDbType.Decimal, in_manifestnum, ParameterDirection.Input);
                cmd.Parameters.Add("in_manifestnum", OracleDbType.Decimal, in_manifestnum, ParameterDirection.Input);

                cmd.Parameters.Add("in_rfid", OracleDbType.Varchar2, in_rfid, ParameterDirection.Input);

                cmd.Parameters.Add("in_scantime", OracleDbType.Date, in_scantime, ParameterDirection.Input);

                cmd.Parameters.Add("in_latitude", OracleDbType.Varchar2, in_latitude, ParameterDirection.Input);

                cmd.Parameters.Add("in_longitude", OracleDbType.Varchar2, in_longitude, ParameterDirection.Input);

                cmd.Parameters.Add("in_temperature", OracleDbType.Single, in_temperature, ParameterDirection.Input);

                cmd.Parameters.Add("in_elevation", OracleDbType.Decimal, in_elevation, ParameterDirection.Input);

                cmd.Parameters.Add("in_relativehumidity", OracleDbType.Single, in_relativehumidity, ParameterDirection.Input);

                cmd.Parameters.Add("initial", OracleDbType.Char, initial, ParameterDirection.Input);

                cmd.Parameters.Add("in_containerrfid", OracleDbType.Varchar2, in_containerrfid, ParameterDirection.Input);

                writeLog(ref cmd);

                cmd.ExecuteNonQuery();

                // run InsertMissing
                cmd.CommandText =
                    "InsertMissing";

                cmd.Parameters.Clear();

                cmd.ArrayBindCount = 0;

                OracleParameter prm10 = cmd.Parameters.Add("in_manifest_num", OracleDbType.Decimal);
                prm10.Direction = ParameterDirection.Input;
                prm10.Value = manifestNum;

                OracleParameter prm11 = cmd.Parameters.Add("in_scantime", OracleDbType.Date);
                prm11.Direction = ParameterDirection.Input;
                prm11.Value = newScantime;

                OracleParameter prm12 = cmd.Parameters.Add("in_latitude", OracleDbType.Varchar2, iP.Latitude, ParameterDirection.Input);

                cmd.Parameters.Add("in_longitude", OracleDbType.Varchar2, iP.Longitude, ParameterDirection.Input);

                cmd.ExecuteNonQuery();

                // now return the manifest, along with descriptions
                cmd.Parameters.Clear();
                cmd.CommandType = CommandType.Text;
                cmd.CommandText =
                    "SELECT scans.rfid,scans.added_removed,descriptions.shortdesc " +
                    "FROM scans,descriptions " +
                    "WHERE scans.scantime=to_date('" + iP.dateTime + "','MM/DD/YY HH:MI:SS AM') " +
                    "AND scans.rfid = descriptions.rfid " +
                    "AND scans.manifest_num=" + manifestNum + " " +
                    "ORDER BY scans.rfid asc";

                writeLog(ref cmd);

                reader = cmd.ExecuteReader();


                while (reader.Read())
                {
                    string currentDesc = reader.GetString(2);

                    if (currentDesc == @"radtag")
                    {
                        //cmd.CommandText = @"RadAlertCall";
                        //cmd.CommandType = CommandType.StoredProcedure;
                        //writeLog(@"Radiation Alert");
                    }
                    else
                    {
                        string currentTag = reader.GetString(0);
                        string addedRemoved;
                        if (!reader.IsDBNull(1))
                            addedRemoved = reader.GetString(1);
                        else
                            addedRemoved = "";

                        int addedRemovedNum;

                        if (addedRemoved == "UNEXPECTED")
                            addedRemovedNum = 1;
                        else if (addedRemoved == "MISSING")
                            addedRemovedNum = -1;
                        else
                            addedRemovedNum = 0;

                        s.SendQueryResponsePacket(new QueryResponse(currentDesc, -1, "", addedRemovedNum, currentTag));
                    }
                }


                s.SendQueryResponsePacket(new QueryResponse("none", int.Parse(manifestNum.ToString()), " ", 0, " "));

                Txn.Commit();
            }
            catch (OracleException e)
            {
                try
                {
                    s.SendQueryResponsePacket(new QueryResponse("ex" + e.Message, -2, " ", 0, " "));
                }
                catch (Exception ex)
                {
                    writeLog("ex " + ex.Message);
                }
                writeLog("ex " + e.Message);
            }
            finally
            {
                if (Txn != null)
                    Txn.Dispose();
                if (cmd != null)
                    cmd.Dispose();
                if (c != null && c.State == ConnectionState.Open)
                    c.Close();
                c.Dispose();
            }
            return;
        }

        private QueryRequest addTag(ref ArrayList inventoryTags, QueryRequest req)
        {
            try
            {
                //byte[] bytes = new byte[12];
                //bytes = req.Rfid.GetBytes();

                inventoryTags.Add(new ListItem(req.Rfid.ToString(), " ", 0));
            }
            catch (ArgumentException ex)
            {
                writeLog(ex.Message);
            }
            return req;
        }

        private void writeLog(string str)
        {
            lock (messagewriter)
            {
                //messagesLock.WaitOne();
                messages.Add(str);
                //System.Console.WriteLine(str);
                EventArgs e = new EventArgs();
                timer1_Tick(this, e);
            }
        }

        private void writeLog(ref OracleCommand cmd)
        {
            writeLog(cmd.CommandText);
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (textBox1.InvokeRequired)
            {
                textBox1.BeginInvoke(new EventHandler(timer1_Tick), new object[] { sender, e });
            }
            else
            {
                //messagesLock.WaitOne();
                lock (messagewriter)
                {
                    if (messages.Count == 0)
                        return;

                    textBox1.BeginUpdate();

                    while (textBox1.Items.Count >= logLimitSize.Value)
                        textBox1.Items.RemoveAt(0);
                    for (int i = 0; i < messages.Count; i++)
                        textBox1.Items.Add(@"[" + DateTime.Now.ToString() + @"] " + messages[i]);

                    textBox1.SelectedIndex = textBox1.Items.Count - 1;

                    textBox1.EndUpdate();

                    messages.Clear();
                }
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            messagesLock.WaitOne();
            textBox1.Items.Clear();
            messagesLock.ReleaseMutex();
        }

        private void textBox1_DoubleClick(object sender, EventArgs e)
        {

            Clipboard.SetDataObject(textBox1.SelectedItem.ToString().Substring(textBox1.SelectedItem.ToString().IndexOf(']') + 2), true);
        }
    }
}