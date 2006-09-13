namespace RFIDProtocolServer
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
			this.components = new System.ComponentModel.Container();
			this.textBox1 = new System.Windows.Forms.ListBox();
			this.usernameBox = new System.Windows.Forms.TextBox();
			this.passwordBox = new System.Windows.Forms.TextBox();
			this.usernameLabel = new System.Windows.Forms.Label();
			this.passwordLabel = new System.Windows.Forms.Label();
			this.dataSourceBox = new System.Windows.Forms.TextBox();
			this.dataSourceLabel = new System.Windows.Forms.Label();
			this.timer1 = new System.Windows.Forms.Timer(this.components);
			this.button1 = new System.Windows.Forms.Button();
			this.label1 = new System.Windows.Forms.Label();
			this.logLimitSize = new System.Windows.Forms.NumericUpDown();
			((System.ComponentModel.ISupportInitialize)(this.logLimitSize)).BeginInit();
			this.SuspendLayout();
			// 
			// textBox1
			// 
			this.textBox1.FormattingEnabled = true;
			this.textBox1.HorizontalScrollbar = true;
			this.textBox1.Location = new System.Drawing.Point(1, 50);
			this.textBox1.Name = "textBox1";
			this.textBox1.Size = new System.Drawing.Size(818, 563);
			this.textBox1.TabIndex = 0;
			this.textBox1.DoubleClick += new System.EventHandler(this.textBox1_DoubleClick);
			// 
			// usernameBox
			// 
			this.usernameBox.Location = new System.Drawing.Point(78, 12);
			this.usernameBox.Name = "usernameBox";
			this.usernameBox.Size = new System.Drawing.Size(71, 20);
			this.usernameBox.TabIndex = 1;
			this.usernameBox.Text = "rfid";
			// 
			// passwordBox
			// 
			this.passwordBox.Location = new System.Drawing.Point(254, 12);
			this.passwordBox.Name = "passwordBox";
			this.passwordBox.PasswordChar = '*';
			this.passwordBox.Size = new System.Drawing.Size(71, 20);
			this.passwordBox.TabIndex = 1;
			this.passwordBox.Text = "rfid#srl#13rfid";
			// 
			// usernameLabel
			// 
			this.usernameLabel.AutoSize = true;
			this.usernameLabel.Location = new System.Drawing.Point(20, 16);
			this.usernameLabel.Name = "usernameLabel";
			this.usernameLabel.Size = new System.Drawing.Size(58, 13);
			this.usernameLabel.TabIndex = 2;
			this.usernameLabel.Text = "Username:";
			// 
			// passwordLabel
			// 
			this.passwordLabel.AutoSize = true;
			this.passwordLabel.Location = new System.Drawing.Point(198, 16);
			this.passwordLabel.Name = "passwordLabel";
			this.passwordLabel.Size = new System.Drawing.Size(56, 13);
			this.passwordLabel.TabIndex = 2;
			this.passwordLabel.Text = "Password:";
			// 
			// dataSourceBox
			// 
			this.dataSourceBox.Location = new System.Drawing.Point(444, 12);
			this.dataSourceBox.Name = "dataSourceBox";
			this.dataSourceBox.Size = new System.Drawing.Size(71, 20);
			this.dataSourceBox.TabIndex = 1;
			this.dataSourceBox.Text = "SRL";
			// 
			// dataSourceLabel
			// 
			this.dataSourceLabel.AutoSize = true;
			this.dataSourceLabel.Location = new System.Drawing.Point(374, 16);
			this.dataSourceLabel.Name = "dataSourceLabel";
			this.dataSourceLabel.Size = new System.Drawing.Size(70, 13);
			this.dataSourceLabel.TabIndex = 2;
			this.dataSourceLabel.Text = "Data Source:";
			// 
			// timer1
			// 
			this.timer1.Enabled = true;
			this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
			// 
			// button1
			// 
			this.button1.Location = new System.Drawing.Point(564, 11);
			this.button1.Name = "button1";
			this.button1.Size = new System.Drawing.Size(71, 23);
			this.button1.TabIndex = 3;
			this.button1.Text = "clear";
			this.button1.UseVisualStyleBackColor = true;
			this.button1.Click += new System.EventHandler(this.button1_Click);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(684, 16);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(46, 13);
			this.label1.TabIndex = 4;
			this.label1.Text = "Log size";
			// 
			// logLimitSize
			// 
			this.logLimitSize.Location = new System.Drawing.Point(730, 12);
			this.logLimitSize.Maximum = new decimal(new int[] {
            4096,
            0,
            0,
            0});
			this.logLimitSize.Name = "logLimitSize";
			this.logLimitSize.Size = new System.Drawing.Size(71, 20);
			this.logLimitSize.TabIndex = 5;
			this.logLimitSize.Value = new decimal(new int[] {
            512,
            0,
            0,
            0});
			// 
			// MainForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(823, 617);
			this.Controls.Add(this.logLimitSize);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.button1);
			this.Controls.Add(this.dataSourceLabel);
			this.Controls.Add(this.passwordLabel);
			this.Controls.Add(this.usernameLabel);
			this.Controls.Add(this.dataSourceBox);
			this.Controls.Add(this.passwordBox);
			this.Controls.Add(this.usernameBox);
			this.Controls.Add(this.textBox1);
			this.Name = "MainForm";
			this.Text = "RFID Protocol Server";
			this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
			this.Load += new System.EventHandler(this.MainForm_Load);
			((System.ComponentModel.ISupportInitialize)(this.logLimitSize)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

		private System.Windows.Forms.ListBox textBox1;
		private System.Windows.Forms.TextBox usernameBox;
		private System.Windows.Forms.TextBox passwordBox;
		private System.Windows.Forms.Label usernameLabel;
		private System.Windows.Forms.Label passwordLabel;
		private System.Windows.Forms.TextBox dataSourceBox;
		private System.Windows.Forms.Label dataSourceLabel;
		private System.Windows.Forms.Timer timer1;
		private System.Windows.Forms.Button button1;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.NumericUpDown logLimitSize;
		//private System.Windows.Forms.Timer timer1;



	}
}

