using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace MPR_Examp_DLL_Proj
{
	/// <summary>
	/// Summary description for FormMsgLog.
	/// </summary>
	public class FormMsgLog : System.Windows.Forms.Form
	{
		private System.Windows.Forms.RichTextBox rtbMsgLog;
		private System.Windows.Forms.ContextMenu contextMenu1;
		private System.Windows.Forms.MenuItem menuItemClearMsgLog;
		private System.Windows.Forms.MenuItem menuItemSaveLog;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public FormMsgLog()
		{
			InitializeComponent();
			AddLine(string.Format("Log started at {0}", DateTime.Now));
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormMsgLog));
            this.rtbMsgLog = new System.Windows.Forms.RichTextBox();
            this.contextMenu1 = new System.Windows.Forms.ContextMenu();
            this.menuItemClearMsgLog = new System.Windows.Forms.MenuItem();
            this.menuItemSaveLog = new System.Windows.Forms.MenuItem();
            this.SuspendLayout();
            // 
            // rtbMsgLog
            // 
            this.rtbMsgLog.BackColor = System.Drawing.SystemColors.Info;
            this.rtbMsgLog.ContextMenu = this.contextMenu1;
            this.rtbMsgLog.Cursor = System.Windows.Forms.Cursors.Default;
            this.rtbMsgLog.Dock = System.Windows.Forms.DockStyle.Fill;
            this.rtbMsgLog.ForeColor = System.Drawing.SystemColors.InfoText;
            this.rtbMsgLog.Location = new System.Drawing.Point(0, 0);
            this.rtbMsgLog.Name = "rtbMsgLog";
            this.rtbMsgLog.ReadOnly = true;
            this.rtbMsgLog.Size = new System.Drawing.Size(600, 608);
            this.rtbMsgLog.TabIndex = 0;
            this.rtbMsgLog.Text = "";
            // 
            // contextMenu1
            // 
            this.contextMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.menuItemClearMsgLog,
            this.menuItemSaveLog});
            // 
            // menuItemClearMsgLog
            // 
            this.menuItemClearMsgLog.Index = 0;
            this.menuItemClearMsgLog.Text = "&Clear Msg Log";
            this.menuItemClearMsgLog.Click += new System.EventHandler(this.menuItemClearMsgLog_Click);
            // 
            // menuItemSaveLog
            // 
            this.menuItemSaveLog.Index = 1;
            this.menuItemSaveLog.Text = "&Save Log...";
            this.menuItemSaveLog.Click += new System.EventHandler(this.menuItemSaveLog_Click);
            // 
            // FormMsgLog
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(600, 608);
            this.Controls.Add(this.rtbMsgLog);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "FormMsgLog";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "FormMsgLog";
            this.Closing += new System.ComponentModel.CancelEventHandler(this.FormMsgLog_Closing);
            this.Load += new System.EventHandler(this.FormMsgLog_Load);
            this.ResumeLayout(false);

		}
		#endregion

		public void Add(string s)
		{
			rtbMsgLog.AppendText(s);
		}

		public void AddLine(string s)
		{
			rtbMsgLog.AppendText(s + "\n");
		}

		private void menuItemClearMsgLog_Click(object sender, System.EventArgs e)
		{
			rtbMsgLog.Clear();
		}

		private void menuItemSaveLog_Click(object sender, System.EventArgs e)
		{
			//rtbMsgLog.SaveFile(FilePath, RichTextBoxStreamType.PlainText);
		}

		private void FormMsgLog_Closing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			e.Cancel = true;
			this.Hide();
		}

		private void FormMsgLog_Load(object sender, System.EventArgs e)
		{
			//this.Show();
		}
	}
}