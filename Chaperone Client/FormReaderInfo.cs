using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using WJ.MPR.Reader;

namespace MPR_Examp_DLL_Proj
{
	/// <summary>
	/// Summary description for FormReaderInfo.
	/// </summary>
	public class FormReaderInfo : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Label lblManufactureDate;
		private System.Windows.Forms.Label label38;
		private System.Windows.Forms.Label lblManufacturer;
		private System.Windows.Forms.Label lbl123;
		private System.Windows.Forms.Label lblModelNumber;
		private System.Windows.Forms.Label label34;
		private System.Windows.Forms.Label lblBLVersion;
		private System.Windows.Forms.Label label30;
		private System.Windows.Forms.Label lblHWVersion;
		private System.Windows.Forms.Label label32;
		private System.Windows.Forms.Label lblSerialNumber;
		private System.Windows.Forms.Label lblRN;
		private System.Windows.Forms.Button btnReadInfo;
		private System.Windows.Forms.Label lblFWVersion;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Button btnClearInfo;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label lblRFPowerMin;
		private System.Windows.Forms.Label lblRFPowerMax;
		private System.Windows.Forms.Button btnDone;

		private MPRReader Reader;

		public FormReaderInfo(MPRReader Reader)
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
			this.Reader = Reader;
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(FormReaderInfo));
			this.lblManufactureDate = new System.Windows.Forms.Label();
			this.label38 = new System.Windows.Forms.Label();
			this.lblManufacturer = new System.Windows.Forms.Label();
			this.lbl123 = new System.Windows.Forms.Label();
			this.lblModelNumber = new System.Windows.Forms.Label();
			this.label34 = new System.Windows.Forms.Label();
			this.lblBLVersion = new System.Windows.Forms.Label();
			this.label30 = new System.Windows.Forms.Label();
			this.lblHWVersion = new System.Windows.Forms.Label();
			this.label32 = new System.Windows.Forms.Label();
			this.lblSerialNumber = new System.Windows.Forms.Label();
			this.lblRN = new System.Windows.Forms.Label();
			this.btnReadInfo = new System.Windows.Forms.Button();
			this.lblFWVersion = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this.btnClearInfo = new System.Windows.Forms.Button();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.lblRFPowerMax = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.lblRFPowerMin = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.btnDone = new System.Windows.Forms.Button();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// lblManufactureDate
			// 
			this.lblManufactureDate.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblManufactureDate.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblManufactureDate.Location = new System.Drawing.Point(176, 104);
			this.lblManufactureDate.Name = "lblManufactureDate";
			this.lblManufactureDate.Size = new System.Drawing.Size(216, 24);
			this.lblManufactureDate.TabIndex = 59;
			// 
			// label38
			// 
			this.label38.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label38.Location = new System.Drawing.Point(8, 104);
			this.label38.Name = "label38";
			this.label38.Size = new System.Drawing.Size(152, 24);
			this.label38.TabIndex = 58;
			this.label38.Text = "Manufacture Date";
			// 
			// lblManufacturer
			// 
			this.lblManufacturer.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblManufacturer.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblManufacturer.Location = new System.Drawing.Point(176, 72);
			this.lblManufacturer.Name = "lblManufacturer";
			this.lblManufacturer.Size = new System.Drawing.Size(216, 24);
			this.lblManufacturer.TabIndex = 57;
			// 
			// lbl123
			// 
			this.lbl123.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lbl123.Location = new System.Drawing.Point(8, 72);
			this.lbl123.Name = "lbl123";
			this.lbl123.Size = new System.Drawing.Size(152, 24);
			this.lbl123.TabIndex = 56;
			this.lbl123.Text = "Manufacturer";
			// 
			// lblModelNumber
			// 
			this.lblModelNumber.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblModelNumber.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblModelNumber.Location = new System.Drawing.Point(176, 40);
			this.lblModelNumber.Name = "lblModelNumber";
			this.lblModelNumber.Size = new System.Drawing.Size(216, 24);
			this.lblModelNumber.TabIndex = 55;
			// 
			// label34
			// 
			this.label34.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label34.Location = new System.Drawing.Point(8, 40);
			this.label34.Name = "label34";
			this.label34.Size = new System.Drawing.Size(152, 24);
			this.label34.TabIndex = 54;
			this.label34.Text = "Model Number";
			// 
			// lblBLVersion
			// 
			this.lblBLVersion.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblBLVersion.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblBLVersion.Location = new System.Drawing.Point(176, 168);
			this.lblBLVersion.Name = "lblBLVersion";
			this.lblBLVersion.Size = new System.Drawing.Size(80, 24);
			this.lblBLVersion.TabIndex = 53;
			// 
			// label30
			// 
			this.label30.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label30.Location = new System.Drawing.Point(8, 136);
			this.label30.Name = "label30";
			this.label30.Size = new System.Drawing.Size(152, 24);
			this.label30.TabIndex = 52;
			this.label30.Text = "Hardware Version";
			// 
			// lblHWVersion
			// 
			this.lblHWVersion.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblHWVersion.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblHWVersion.Location = new System.Drawing.Point(176, 136);
			this.lblHWVersion.Name = "lblHWVersion";
			this.lblHWVersion.Size = new System.Drawing.Size(120, 24);
			this.lblHWVersion.TabIndex = 51;
			// 
			// label32
			// 
			this.label32.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label32.Location = new System.Drawing.Point(8, 168);
			this.label32.Name = "label32";
			this.label32.Size = new System.Drawing.Size(160, 24);
			this.label32.TabIndex = 50;
			this.label32.Text = "Bootloader Version";
			// 
			// lblSerialNumber
			// 
			this.lblSerialNumber.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblSerialNumber.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblSerialNumber.Location = new System.Drawing.Point(176, 8);
			this.lblSerialNumber.Name = "lblSerialNumber";
			this.lblSerialNumber.Size = new System.Drawing.Size(216, 24);
			this.lblSerialNumber.TabIndex = 49;
			// 
			// lblRN
			// 
			this.lblRN.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblRN.Location = new System.Drawing.Point(8, 8);
			this.lblRN.Name = "lblRN";
			this.lblRN.Size = new System.Drawing.Size(152, 24);
			this.lblRN.TabIndex = 48;
			this.lblRN.Text = "Serial Number";
			// 
			// btnReadInfo
			// 
			this.btnReadInfo.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.btnReadInfo.Location = new System.Drawing.Point(424, 8);
			this.btnReadInfo.Name = "btnReadInfo";
			this.btnReadInfo.Size = new System.Drawing.Size(152, 48);
			this.btnReadInfo.TabIndex = 45;
			this.btnReadInfo.Text = "Read MPR Info";
			this.btnReadInfo.Click += new System.EventHandler(this.btnReadInfo_Click);
			// 
			// lblFWVersion
			// 
			this.lblFWVersion.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblFWVersion.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblFWVersion.Location = new System.Drawing.Point(176, 200);
			this.lblFWVersion.Name = "lblFWVersion";
			this.lblFWVersion.Size = new System.Drawing.Size(80, 24);
			this.lblFWVersion.TabIndex = 47;
			// 
			// label4
			// 
			this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label4.Location = new System.Drawing.Point(8, 200);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(144, 24);
			this.label4.TabIndex = 46;
			this.label4.Text = "Firmware Version";
			// 
			// btnClearInfo
			// 
			this.btnClearInfo.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.btnClearInfo.Location = new System.Drawing.Point(424, 80);
			this.btnClearInfo.Name = "btnClearInfo";
			this.btnClearInfo.Size = new System.Drawing.Size(152, 48);
			this.btnClearInfo.TabIndex = 60;
			this.btnClearInfo.Text = "Clear MPR Info";
			this.btnClearInfo.Click += new System.EventHandler(this.btnClearInfo_Click);
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.lblRFPowerMax);
			this.groupBox1.Controls.Add(this.label5);
			this.groupBox1.Controls.Add(this.lblRFPowerMin);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.groupBox1.Location = new System.Drawing.Point(0, 240);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(584, 88);
			this.groupBox1.TabIndex = 61;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "RF Power Limits (dBm)";
			// 
			// lblRFPowerMax
			// 
			this.lblRFPowerMax.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblRFPowerMax.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblRFPowerMax.Location = new System.Drawing.Point(456, 40);
			this.lblRFPowerMax.Name = "lblRFPowerMax";
			this.lblRFPowerMax.Size = new System.Drawing.Size(80, 24);
			this.lblRFPowerMax.TabIndex = 51;
			// 
			// label5
			// 
			this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label5.Location = new System.Drawing.Point(312, 40);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(144, 24);
			this.label5.TabIndex = 50;
			this.label5.Text = "Maximum";
			// 
			// lblRFPowerMin
			// 
			this.lblRFPowerMin.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblRFPowerMin.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblRFPowerMin.Location = new System.Drawing.Point(176, 40);
			this.lblRFPowerMin.Name = "lblRFPowerMin";
			this.lblRFPowerMin.Size = new System.Drawing.Size(80, 24);
			this.lblRFPowerMin.TabIndex = 49;
			// 
			// label2
			// 
			this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label2.Location = new System.Drawing.Point(16, 40);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(144, 24);
			this.label2.TabIndex = 48;
			this.label2.Text = "Minimum";
			// 
			// btnDone
			// 
			this.btnDone.Font = new System.Drawing.Font("Microsoft Sans Serif", 10.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.btnDone.Location = new System.Drawing.Point(424, 176);
			this.btnDone.Name = "btnDone";
			this.btnDone.Size = new System.Drawing.Size(152, 48);
			this.btnDone.TabIndex = 62;
			this.btnDone.Text = "Done";
			// 
			// FormReaderInfo
			// 
			this.AcceptButton = this.btnDone;
			this.AutoScaleBaseSize = new System.Drawing.Size(6, 15);
			this.CancelButton = this.btnDone;
			this.ClientSize = new System.Drawing.Size(584, 328);
			this.Controls.Add(this.btnDone);
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.btnClearInfo);
			this.Controls.Add(this.lblManufactureDate);
			this.Controls.Add(this.label38);
			this.Controls.Add(this.lblManufacturer);
			this.Controls.Add(this.lbl123);
			this.Controls.Add(this.lblModelNumber);
			this.Controls.Add(this.label34);
			this.Controls.Add(this.lblBLVersion);
			this.Controls.Add(this.label30);
			this.Controls.Add(this.lblHWVersion);
			this.Controls.Add(this.label32);
			this.Controls.Add(this.lblSerialNumber);
			this.Controls.Add(this.lblRN);
			this.Controls.Add(this.btnReadInfo);
			this.Controls.Add(this.lblFWVersion);
			this.Controls.Add(this.label4);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "FormReaderInfo";
			this.ShowInTaskbar = false;
			this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "MPR Manufacturing Information";
			this.Load += new System.EventHandler(this.FormReaderInfo_Load);
			this.groupBox1.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		private void UpdateInfo()
		{
			Reader.UpdateReaderInfo();
			lblManufactureDate.Text = Reader.ManufactureDate;
			lblManufacturer.Text = Reader.ManufacturerName;
			lblModelNumber.Text = Reader.DeviceModelNumber;
			lblBLVersion.Text = Reader.BootLoaderVersion;
			lblHWVersion.Text = Reader.HardwareVersion;
			lblSerialNumber.Text = Reader.DeviceSerialNumber;
			lblFWVersion.Text = Reader.FirmwareVersion;

			lblRFPowerMin.Text = Reader.TxPowerMin.ToString();
			lblRFPowerMax.Text = Reader.TxPowerMax.ToString();
		}

		private void btnReadInfo_Click(object sender, System.EventArgs e)
		{
			btnReadInfo.Enabled = false;
			UpdateInfo();
			btnReadInfo.Enabled = true;
		}

		private void btnClearInfo_Click(object sender, System.EventArgs e)
		{
			lblManufactureDate.Text = "";
			lblManufacturer.Text = "";
			lblModelNumber.Text = "";
			lblBLVersion.Text = "";
			lblHWVersion.Text = "";
			lblSerialNumber.Text = "";
			lblFWVersion.Text = "";

			lblRFPowerMin.Text = "";
			lblRFPowerMax.Text = "";
		}

		private void FormReaderInfo_Load(object sender, System.EventArgs e)
		{
			UpdateInfo();
		}
	}
}
