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
		Daemon d;
		bool isRunning;
		int port = 5060;
		string PhoneNumber = "3012330583";

		private ArrayList messages;
		private Mutex messagesLock;

		private const string DBSERVER = "SRL";
		private const string DATABASE = "RFID";

		public string[] a = new string[5];

		public MainForm()
		{
			InitializeComponent();
			messages = new ArrayList();
			messagesLock = new Mutex();
		}

		private void MainForm_Load(object sender, EventArgs e)
		{
			isRunning = true;

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
				d = new Daemon(Daemon.PORT);
				writeLog("Starting Daemon...");
				d.Start();
				d.BeginAcceptClientConnection(new AsyncCallback(AcceptClient), d);
				writeLog("Daemon listening...");

				while (isRunning)
				{
					Thread.Sleep(1000);
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
					if (!s.GetStream().DataAvailable)
					{
						Thread.Sleep(5);
						continue;
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
								QueryRequest req = new QueryRequest(newPacket.Value);

								req = addTag(ref inventoryTags, req);
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
									s.sendAck();
									// now go do requests
									c = new OracleConnection(MyConString);
									c.Open();

									cmd = new OracleCommand();
									cmd.Connection = c;

									cmd.CommandText = "InsertAlerts";

									cmd.CommandType = CommandType.StoredProcedure;

									OracleParameter prm1 = cmd.Parameters.Add("in_containerrfid", OracleDbType.Varchar2);
									prm1.Direction = ParameterDirection.Input;
									prm1.Value = req.containerRFID;

									OracleParameter prm2 = cmd.Parameters.Add("in_alertdt", OracleDbType.Date);
									prm2.Direction = ParameterDirection.Input;
									prm2.Value = new OracleDate(req.dateTime); ;

									OracleParameter prm3 = cmd.Parameters.Add("in_numbertocall", OracleDbType.Varchar2);
									prm3.Direction = ParameterDirection.Input;
									prm3.Value = req.numberToCall;

									writeLog(cmd.CommandText + "," + req.containerRFID + "," + req.dateTime + "," + req.numberToCall);
									writeLog(cmd.CommandText + "," + prm1.Value + "," + prm2.Value + "," + prm3.Value);

									cmd.ExecuteNonQuery();
								}
								catch (Exception e)
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
								//s.SendRaiseAlertPacket(new RaiseAlertResponse());

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

								writeLog(ref cmd);

								cmd.ExecuteNonQuery();

								c.Close();
								c.Dispose();
								VoIP.VoIPConnection voip = new VoIP.VoIPConnection("3796001", "bello", "inphonex.com", port.ToString(), "300");
								voip.call(PhoneNumber, "temp1.raw");
								port++;
							} break;

						#endregion

						#region InfoHandler
						case (ushort)Packets.Info: //Info Type, also signals the end of the tag stream
							{
								Info iP = new Info(newPacket.Value);

								checkinItems(s, MyConString, ref inventoryTags, iP);

								s.Close();
							} break;
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
							break;
					}
				}
			}
			catch (Exception e)
			{
				writeLog(e.Message.ToString());
			}
			s.Close();
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
				Txn = c.BeginTransaction(IsolationLevel.ReadCommitted);

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
						"FROM scans";
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
				}

				cmd.BindByName = true;

				cmd.CommandType = CommandType.StoredProcedure;

				cmd.ArrayBindCount = inventoryTags.Count;

				// run InsertScans
				cmd.CommandText =
					"InsertScans";

				writeLog(cmd.CommandText);

				OracleParameter prm1 = cmd.Parameters.Add("in_manifestnum", OracleDbType.Decimal);
				prm1.Direction = ParameterDirection.Input;
				prm1.Value = in_manifestnum;

				OracleParameter prm2 = cmd.Parameters.Add("in_rfid", OracleDbType.Varchar2);
				prm2.Direction = ParameterDirection.Input;
				prm2.Value = in_rfid;

				OracleParameter prm3 = cmd.Parameters.Add("in_scantime", OracleDbType.Date);
				prm3.Direction = ParameterDirection.Input;
				prm3.Value = in_scantime;

				OracleParameter prm4 = cmd.Parameters.Add("in_latitude", OracleDbType.Varchar2);
				prm4.Direction = ParameterDirection.Input;
				prm4.Value = in_latitude;

				OracleParameter prm5 = cmd.Parameters.Add("in_longitude", OracleDbType.Varchar2);
				prm5.Direction = ParameterDirection.Input;
				prm5.Value = in_longitude;

				OracleParameter prm6 = cmd.Parameters.Add("in_temperature", OracleDbType.Single);
				prm6.Direction = ParameterDirection.Input;
				prm6.Value = in_temperature;

				OracleParameter prm7 = cmd.Parameters.Add("in_elevation", OracleDbType.Decimal);
				prm7.Direction = ParameterDirection.Input;
				prm7.Value = in_elevation;

				OracleParameter prm8 = cmd.Parameters.Add("in_relativehumidity", OracleDbType.Single);
				prm8.Direction = ParameterDirection.Input;
				prm8.Value = in_relativehumidity;

				OracleParameter prm9 = cmd.Parameters.Add("initial", OracleDbType.Char);
				prm9.Direction = ParameterDirection.Input;
				prm9.Value = initial;

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

				if (reader.HasRows)
				{
					while (reader.Read())
					{
						string currentDesc = reader.GetString(2);

						if (currentDesc == @"radtag")
						{
							cmd.CommandText = @"RadAlertCall";
							cmd.CommandType = CommandType.StoredProcedure;
							writeLog(@"Radiation Alert");
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
				}				

				s.SendQueryResponsePacket(new QueryResponse("none", int.Parse(manifestNum.ToString()), " ", 0, " "));

				Txn.Commit();
			}
			catch (Exception e)
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
				byte[] bytes = new byte[12];
				bytes = req.Rfid.GetBytes();
				//bytes[11] += 0x18;
				req.Rfid = new RFID(bytes);

				inventoryTags.Add(new ListItem(new RFID(bytes)));
			}
			catch (ArgumentException ex)
			{
				writeLog(ex.Message);
			}
			return req;
		}

		private void writeLog(string str)
		{			
			messagesLock.WaitOne();
			messages.Add(str);
			System.Console.WriteLine(str);			
			messagesLock.ReleaseMutex();
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
				messagesLock.WaitOne();
				while (textBox1.Items.Count >= 2048)
					textBox1.Items.RemoveAt(0);
				for (int i = 0; i < messages.Count; i++)
					textBox1.Items.Add(messages[i]);
				messages.Clear();
				messagesLock.ReleaseMutex();
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
			Clipboard.SetDataObject(textBox1.SelectedItem,true);
		}
	}
}