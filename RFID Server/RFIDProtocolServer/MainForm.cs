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

			string MyConString = "User Id=" + usernameBox.Text + ";" +
				"Password=" + passwordBox.Text + ";" +
				"Data Source=" + dataSourceBox.Text;
					
			int numberOfQueries = -1;
			ArrayList inventoryTags = new ArrayList();
			bool done = false;

			s.WaitForConnectPacket();
			Console.Out.WriteLine("Got connect packet.");
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

								inventoryTags.Add(new ListItem(new RFID(bytes)));
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

								string currentTime = DateTime.Now.ToString();

								cmd.CommandText = "INSERT INTO alerts VALUES(alertnum_seq.nextval" +
									",'" + "313233343536000000000046" +
									"','" + PhoneNumber.ToString() +
									"',2,0,'root@localhost','Radiation alert.'," + "'313233343536000000000046'" +
									"TO_DATE('" + currentTime + "','MM/DD/YY HH:MI:SS AM'))" + ")";
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
									Txn = c.BeginTransaction(IsolationLevel.ReadCommitted);

									cmd = new OracleCommand();
									cmd.Connection = c;

									// determine the container's RFID
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

									reader.Read();
									string containerRFID = reader.GetString(0);

									decimal manifestNum;

									// if a new manifest needs to be made, do so, else
									// just add to the table
									if (iP.isScan == 0)
									{
										// get a new manifestnum for the parent
										cmd.CommandText =
											"INSERT INTO manifest " +
											"VALUES( manifestnum_seq.nextval,'" +
											containerRFID + "',TO_DATE('" +
											iP.dateTime + "','MM/DD/YY HH:MI:SS AM'))";

										writeLog(ref cmd);

										cmd.ExecuteNonQuery();

										// insert items as child scans
										for (int i = 0; i < inventoryTags.Count; ++i)
										{
											cmd.CommandText =
												"INSERT INTO scans " +
												"VALUES(manifestnum_seq.currval,'" +
												inventoryTags[i].ToString() + "','',TO_DATE('" +
												iP.dateTime + "','MM/DD/YY HH:MI:SS AM'),'" +
												iP.Latitude + "','" +
												iP.Longitude + "'," +
												"0,0,0)";

											writeLog(ref cmd);

											cmd.ExecuteNonQuery();
										}

										// get the manifest number, to be used in retrieving the whole manifest later
										cmd.CommandText =
											"SELECT manifestnum_seq.currval FROM scans";

										writeLog(ref cmd);

										reader = cmd.ExecuteReader();

										if (reader.Read())
											manifestNum = reader.GetDecimal(0);
										else
										{
											writeLog("Scan couldn't generate a manifest_num");
											return;
										}
										reader.Close();
									}
									else // then it is a scan and does not create a manifest, but does need to look for added/missing items 
									{
										// get manifest number based on container RFID
										cmd.CommandText =
											"SELECT max(manifest_num) " +
											"FROM manifest " +
											"WHERE rfid = '" + containerRFID + "'";

										writeLog(ref cmd);

										reader = cmd.ExecuteReader();
										reader.Read();

										if (!reader.IsDBNull(0))
											manifestNum = reader.GetDecimal(0);
										else
										{
											writeLog("Subsequent scan without container tag");
											return;
										}
										
										// insert items as child scans, add missing items later
										// mark items as unexpected later also
										for (int i = 0; i < inventoryTags.Count; ++i)
										{
											cmd.CommandText = "INSERT INTO scans " +
												"VALUES( " + manifestNum.ToString() + ",'" +
												inventoryTags[i].ToString() + "','',TO_DATE('" +
												iP.dateTime + "','MM/DD/YY HH:MI:SS AM'),'" +
												iP.Latitude + "','" +
												iP.Longitude + "'," +
												"0,0,0)";

											writeLog(ref cmd);

											cmd.ExecuteNonQuery();
										}

										// do the updates using a stored procedure
										cmd.CommandText = "UpdateScans";

										cmd.BindByName = true;
										
										cmd.CommandType = CommandType.StoredProcedure;
										
										OracleParameter prm1 = cmd.Parameters.Add("in_manifest_num", OracleDbType.Decimal);
										prm1.Direction = ParameterDirection.Input;
										prm1.Value = manifestNum;

										OracleDate in_scantime = new OracleDate(iP.dateTime);
										OracleParameter prm2 = cmd.Parameters.Add("in_scantime", OracleDbType.Date);
										prm2.Direction = ParameterDirection.Input;
										prm2.Value = in_scantime;

										writeLog(ref cmd);

										cmd.ExecuteNonQuery();
									}

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

									if (reader.HasRows)
									{
										while (reader.Read())
										{
											string currentTag = reader.GetString(0);
											string addedRemoved;
											if (!reader.IsDBNull(1))
												addedRemoved = reader.GetString(1);
											else
												addedRemoved = "";
											string currentDesc = reader.GetString(2);
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
									s.SendQueryResponsePacket(new QueryResponse(" ", int.Parse(manifestNum.ToString()), " ", 0, " "));
									
									Txn.Commit();
								}
								catch (Exception e)
								{
									writeLog(e.Message);
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

							} break;

						case addRemoveItem.Type:

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

						default:
							{
								throw new Exception("Bad protocol type");
							}
							break;
					}
				}
			}
			catch (Exception e)
			{
				writeLog(e.Message.ToString());
			}

			s.Close();
			//Console.Out.WriteLine("Client closed");
			writeLog("Client closed");
		}

		private void writeLog(string str)
		{
			System.Console.WriteLine(str);
			messagesLock.WaitOne();
			messages.Add(str);
			messagesLock.ReleaseMutex();
		}

		private void writeLog(ref OracleCommand cmd)
		{
			System.Console.WriteLine(cmd.CommandText);
			messagesLock.WaitOne();
			messages.Add(cmd.CommandText);
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