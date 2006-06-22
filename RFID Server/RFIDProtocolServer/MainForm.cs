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


using RFIDProtocolLib;

namespace RFIDProtocolServer
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
			messages = new ArrayList();
			messagesLock = new Mutex();
        }

        Daemon d;
        bool isRunning;
		int port = 5060;
        string PhoneNumber = "3012330583";

		private ArrayList messages;
		private Mutex messagesLock;

		private const string DBSERVER = "SRL";
		private const string DATABASE = "RFID";

		//private static OracleConnection c;

        private void MainForm_Load(object sender, EventArgs e)
        {
            isRunning = true;

            Thread t = new Thread(new ThreadStart(RunDaemon));
            t.Start();
			//timer1.Interval = 1000;
			//timer1.`
			timer1.Enabled = true;
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            isRunning = false;
        }

        private void RunDaemon()
        {
            try
            {

                d = new Daemon(Daemon.PORT);
                Console.Out.WriteLine("Starting Daemon...");
                d.Start();
                d.BeginAcceptClientConnection(new AsyncCallback(AcceptClient), d);
                Console.Out.WriteLine("Daemon listening...");

                while (isRunning)
                {
                    Thread.Sleep(10);
                }
            }
            catch (Exception)
            {
            }

            Console.Out.WriteLine("Closing Daemon...");

            d.Stop();
        }

        private void AcceptClient(IAsyncResult result)
        {
            Console.Out.WriteLine("Received new client");
            Daemon d = (Daemon)result.AsyncState;
            ServerConnection s = d.EndAcceptClientConnection(result);
            Thread t = new Thread(new ParameterizedThreadStart(RunServer));
            t.Start(s);

            d.BeginAcceptClientConnection(new AsyncCallback(AcceptClient), d);
        }

        private void RunServer(object obj)
        {
            Console.Out.WriteLine("Client thread running");
            ServerConnection s = (ServerConnection)obj;

			string MyConString = "User Id=" + usernameBox.Text +";" +
				"Password=" + passwordBox.Text + ";" +
				"Data Source=" + dataSourceBox.Text;

			string Latitude = "00 00.00N";
			string Longitude = "00 00.00W";
			int IsScan = 0;

            int numberOfQueries = -1;
			ArrayList inventoryTags = new ArrayList();
			bool done = false;

            s.WaitForConnectPacket();
			Console.Out.WriteLine("Got conntect packet.");
            s.SendConnectResponsePacket();
			Console.Out.WriteLine("Sent response connect.");

            try
            {
                while (isRunning && s.Connected && !done)
                {
                    if (!s.GetStream().DataAvailable)
                    {
                        Thread.Sleep(5);
                        continue;
                    }

                    TLV newPacket = new TLV();

                    newPacket.ReadFromStream(s.GetStream());


					if (newPacket.Type == 0)
					{
						continue;
					}

                    switch (newPacket.Type)
                    {
                        case QueryRequest.Type:
                            {
                                numberOfQueries += 1;
                                QueryRequest req = new QueryRequest(newPacket.Value);
								byte[] bytes = new byte[12];
								bytes = req.Rfid.GetBytes();
								bytes[11] += 0x18;
								req.Rfid = new RFID(bytes);
                                //System.Console.WriteLine("Querying for {0}", req.Rfid.ToString());
								inventoryTags.Add(new ListItem(new RFID(bytes)));
                                //QueryResponse response = DBBackend.QueryRFID(req, s);
                                //System.Console.WriteLine("Writing back {0}", response.ShortDesc);
                                //s.SendQueryResponsePacket(response);
                            } break;
                        case SetPhoneNumberRequest.Type:
                            {
                                SetPhoneNumberRequest req = new SetPhoneNumberRequest(newPacket.Value);
								if (req.PhoneNumber == "00000000")
								{
									done = true;
									break;
								}
                                System.Console.WriteLine("Setting phone # to {0}", req.PhoneNumber);
                                PhoneNumber = req.PhoneNumber;
                                SetPhoneNumberResponse resp = new SetPhoneNumberResponse();
                                s.SendSetPhoneNumberPacket(resp);
                            } break;
                        case RaiseAlertRequest.Type:
                            {
								s.SendRaiseAlertPacket(new RaiseAlertResponse());

								// now go do requests
								OracleConnection c = new OracleConnection(MyConString);
								c.Open();

								OracleCommand cmd = new OracleCommand();
								cmd.Connection = c;                                

								cmd.CommandText = "INSERT INTO alerts VALUES(alertnum_seq.nextval" +
									",'" + "313233343536000000000046" +
									"','" + PhoneNumber.ToString() +
									"',2,0,'root@localhost','Radiation alert.'," + "'313233343536000000000046'" + ")";
								System.Console.WriteLine(cmd.CommandText);
								cmd.ExecuteNonQuery();

								cmd.CommandText = "commit";
								System.Console.WriteLine(cmd.CommandText);

								cmd.ExecuteNonQuery();

								c.Close();
								c.Dispose();
                                VoIP.VoIPConnection voip = new VoIP.VoIPConnection("3796001", "bello", "inphonex.com", port.ToString(), "300");
                                voip.call(PhoneNumber, "temp1.raw");
								port++;
                            } break;
						case InfoPacket.Type: //InfoPacket Type, also signals the end of the tag stream
							{
								InfoPacket iP = new InfoPacket(newPacket.Value);
								Latitude = iP.Latitude;
								Longitude = iP.Longitude;
								IsScan = iP.isScan;								
							
								try
								{
									// now go do requests
									OracleConnection c = new OracleConnection(MyConString);
									c.Open();

									// first figure out which of these is a container
									int containerFound = -1;
									OracleCommand cmd = new OracleCommand();
									cmd.Connection = c;

									for (int i = 0; i < inventoryTags.Count; ++i)
									{
										cmd.CommandText = "SELECT shortDesc " +
											"FROM Descriptions WHERE rfid='" +
											inventoryTags[i].ToString() + "'";
										System.Console.WriteLine(cmd.CommandText);

										messagesLock.WaitOne();
										messages.Add(cmd.CommandText);
										messagesLock.ReleaseMutex();
										

										OracleDataReader reader = cmd.ExecuteReader();
										while (reader.Read())
											((ListItem)inventoryTags[i]).shortDesc = (string)reader.GetString(0);
											if ((containerFound == -1) && (((ListItem)inventoryTags[i]).shortDesc == "Shipping Container 42D"))
											{
												System.Console.WriteLine(inventoryTags[i].ToString() + " is a container");
												containerFound = i;
												continue;
											}
										System.Console.WriteLine(inventoryTags[i].ToString());

										messagesLock.WaitOne();
										messages.Add(inventoryTags[i].ToString());
										messagesLock.ReleaseMutex();
									}

									ArrayList oldInvList = new ArrayList(); // the items checked in or added in the last scan
									
									// if a new manifest needs to be made, do so, else
									// just add to the table
									if (containerFound >= 0)
									{
										string currentTime = DateTime.Now.ToString();

										if (IsScan == 0) // time to autogen a new manifest
										{
											// get a new manifestnum for the parent
											cmd.CommandText = "INSERT INTO manifest " +
												"VALUES( manifestnum_seq.nextval,'" +
												inventoryTags[containerFound].ToString() + "',TO_DATE('" +
												currentTime + "','MM/DD/YY HH:MI:SS AM'))";
											System.Console.WriteLine(cmd.CommandText);

											messagesLock.WaitOne();
											messages.Add(cmd.CommandText);
											messagesLock.ReleaseMutex();

											cmd.ExecuteNonQuery();

											// insert items as child scans
											for (int i = 0; i < inventoryTags.Count; ++i)
											{
												cmd.CommandText = "INSERT INTO scans " +
													"VALUES( manifestnum_seq.currval,'" +
													inventoryTags[i].ToString() + "','',TO_DATE('" +
													currentTime + "','MM/DD/YYYY HH:MI:SS AM'),'" +
													Latitude + "','" +
													Longitude + "'," +
													"0,0,0)";
												System.Console.WriteLine(cmd.CommandText);
												messagesLock.WaitOne();
												messages.Add(cmd.CommandText);
												messagesLock.ReleaseMutex();
												cmd.ExecuteNonQuery();
											}
										}
										else // else make sure nothing is missing, then add items
										{
											// first determine the manifest number for this shipment
											cmd.CommandText = "SELECT max(manifest_num) " +
												"FROM manifest " +
												"WHERE rfid = '" + inventoryTags[containerFound].ToString() + "'";
											System.Console.WriteLine(cmd.CommandText);
											messagesLock.WaitOne();
											messages.Add(cmd.CommandText);
											messagesLock.ReleaseMutex();
											OracleDataReader reader = cmd.ExecuteReader();
											reader.Read();
											decimal manifestNum;
											
											if (!reader.IsDBNull(0))
												manifestNum = reader.GetDecimal(0);
											else
											{
												System.Console.WriteLine("Subsequent scan without manifest");
												messagesLock.WaitOne();
												messages.Add("Subsequent scan without manifest");
												messagesLock.ReleaseMutex();
												return;
											}
											reader.Close();

											// then get the manifest as updated at the last stop
											cmd.CommandText = "SELECT rfid " +
												"FROM scans " +
												"WHERE manifest_num = " + manifestNum.ToString() + " " +
												"AND (added_removed != 'Removed' OR added_removed is null) " +
												"AND scantime = (SELECT MAX(scantime) " +
												"FROM scans " +
												"WHERE manifest_num = " + manifestNum.ToString() + ")";
											System.Console.WriteLine(cmd.CommandText);
											messagesLock.WaitOne();
											messages.Add(cmd.CommandText);
											messagesLock.ReleaseMutex();

											reader = cmd.ExecuteReader();

											// build an arraylist of the items from the last scan
											while (reader.Read())
											{
												if (!reader.IsDBNull(0))
													oldInvList.Add(new ListItem(new RFID(reader.GetString(0))));												
											}
											reader.Close();

											ArrayList checkedIn = new ArrayList();

											// remove the items from this scan from the list of items
											// in the last scan, leaving the missing items
											for (int jj = 0; jj < inventoryTags.Count; ++jj)
											{
												((ListItem)inventoryTags[jj]).missingAdded = 1;
												for (int j = 0; j < oldInvList.Count; ++j)
													if (oldInvList[j].ToString() == inventoryTags[jj].ToString())
													{
														checkedIn.Add(inventoryTags[jj]);
														((ListItem)inventoryTags[jj]).missingAdded = 0;
														oldInvList.RemoveAt(j);
														break;
													}
											}

											// if anything is missing, create an alert for now
											// later this will possibly be "removed"
											for (int j = 0; j < oldInvList.Count; ++j)
											{
												cmd.CommandText =
													"INSERT INTO alerts VALUES(alertnum_seq.nextval" +
													",'" + inventoryTags[containerFound].ToString() +
													"','" + PhoneNumber.ToString() +
													"',2,0,'root@localhost','From " + inventoryTags[containerFound].ToString() +
													" RFID " + oldInvList[j].ToString() + "is missing.'," + oldInvList[j].ToString() + ")";												
												System.Console.WriteLine(cmd.CommandText);
												messagesLock.WaitOne();
												messages.Add(cmd.CommandText);
												messagesLock.ReleaseMutex();
												cmd.ExecuteNonQuery();
											}
											// insert items as child scans
											for (int i = 0; i < checkedIn.Count; ++i)
											{
												cmd.CommandText = "INSERT INTO scans " +
													"VALUES( " + manifestNum.ToString() + ",'" +
													checkedIn[i].ToString() + "','',TO_DATE('" +
													currentTime + "','MM/DD/YYYY HH:MI:SS AM'),'" +
													Latitude + "','" +
													Longitude + "'," +
													"0,0,0)";
												System.Console.WriteLine(cmd.CommandText);
												messagesLock.WaitOne();
												messages.Add(cmd.CommandText);
												messagesLock.ReleaseMutex();
												cmd.ExecuteNonQuery();
											}										
										}
										cmd.CommandText = "commit";
										System.Console.WriteLine(cmd.CommandText);
										messagesLock.WaitOne();
										messages.Add(cmd.CommandText);
										messagesLock.ReleaseMutex();
										cmd.ExecuteNonQuery();
									}

									// finally, send back the short descriptions
									for (int k = 0; k < inventoryTags.Count; ++k)
									{
										if (k != inventoryTags.Count - 1)
											s.SendQueryResponsePacket(new QueryResponse(((ListItem)inventoryTags[k]).shortDesc, "", "", true, ((ListItem)inventoryTags[k]).missingAdded, inventoryTags[k].ToString()));
										else
										{
											if (oldInvList.Count > 0)
												s.SendQueryResponsePacket(new QueryResponse(((ListItem)inventoryTags[k]).shortDesc, "", "", true, ((ListItem)inventoryTags[k]).missingAdded, inventoryTags[k].ToString()));
											else
												s.SendQueryResponsePacket(new QueryResponse(((ListItem)inventoryTags[k]).shortDesc, "", "", false, ((ListItem)inventoryTags[k]).missingAdded, inventoryTags[k].ToString()));
										}
									}
									for (int l = 0; l < oldInvList.Count; ++l)
									{
										cmd.CommandText = "SELECT shortDesc FROM descriptions WHERE rfid='" + oldInvList[l].ToString() + "'";
										OracleDataReader reader = cmd.ExecuteReader();
										messagesLock.WaitOne();
										messages.Add(cmd.CommandText);
										messagesLock.ReleaseMutex();
										reader.Read();
										string desc;
										if (!reader.IsDBNull(0))
											desc = reader.GetString(0);
										else
											desc = oldInvList[l].ToString();

										if (l != oldInvList.Count -1)
											s.SendQueryResponsePacket(new QueryResponse(desc, "", "",true,-1,oldInvList[l].ToString()));
										else
											s.SendQueryResponsePacket(new QueryResponse(desc, "", "", false, -1, oldInvList[l].ToString()));
									}
									inventoryTags.Clear();
									c.Close();
									c.Dispose();
								}
								catch (Exception e)
								{
									System.Console.Out.WriteLine(e.Message);
								}
								
							}break;
						default:
							{
								throw new Exception("Bad protocol type");
							} break;
                    }
                }
            }
            catch (Exception e)
            {
				Console.Out.WriteLine(e.Message);
				messagesLock.WaitOne();
				messages.Add(e.Message);
				messagesLock.ReleaseMutex();
            }

            /*if (numberOfQueries > 0 && numberOfQueries != 4)
            {
                Console.Out.WriteLine("Manifest wrong!");
                DBBackend.Status = 3;
                VoIP.VoIPConnection voip = new VoIP.VoIPConnection("3796001", "bello", "inphonex.com", "5060", "300");
                voip.call(PhoneNumber, "temp1.raw");
            } */

            s.Close();
            Console.Out.WriteLine("Client closed");
			messagesLock.WaitOne();
			messages.Add("Client Closed");
			messagesLock.ReleaseMutex();
        }

		private void timer1_Tick(object sender, EventArgs e)
		{
			if (textBox1.InvokeRequired)
			{
				textBox1.BeginInvoke(new EventHandler(timer1_Tick), new object[] { sender, e });
			}
			else
			{
				messagesLock.WaitOne();
				for (int i = 0; i < messages.Count; i++)
					textBox1.Items.Add(messages[i]);
				messages.Clear();
				messagesLock.ReleaseMutex();
			}
		}

    }

	//ListItem class
	public class ListItem
	{
		private string ShortDesc = "";
		private RFID rfidNum = new RFID("0");
		private int imageIndex = -1;
		private int MissingAdded = 0;

		public ListItem(string rfid, string shortdesc, int b)
		{
			rfidNum = new RFID(rfid);
			ShortDesc = shortdesc;
			MissingAdded = b;
		}

		public ListItem(RFID rfid)
		{
			rfidNum = rfid;
		}

		public override string ToString()
		{
			return rfidNum.ToString();
		}

		public string shortDesc
		{
			get
			{
				return ShortDesc;
			}
			set
			{
				ShortDesc = value;
			}
		}

		public string RFIDNum
		{
			get
			{
				return rfidNum.ToString();
			}
			set
			{
				rfidNum = new RFID(value);
			}
		}

		public int ImageIndex
		{
			get
			{
				return imageIndex;
			}
			set
			{
				imageIndex = value;
			}
		}

		public int missingAdded
		{
			get
			{
				return MissingAdded;
			}
			set
			{
				MissingAdded = value;
			}
		}
	}
}