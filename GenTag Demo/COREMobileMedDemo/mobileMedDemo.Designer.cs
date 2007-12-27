namespace COREMobileMedDemo
{
    partial class mobileMedDemo
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
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.button3 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.patientVitalsListView = new System.Windows.Forms.ListView();
            this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
            this.DOBLabel = new System.Windows.Forms.Label();
            this.patientNameLabel = new System.Windows.Forms.Label();
            this.medPB = new System.Windows.Forms.PictureBox();
            this.patientPhotoPB = new System.Windows.Forms.PictureBox();
            this.readMedButton = new System.Windows.Forms.Button();
            this.readPatientButton = new System.Windows.Forms.Button();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.SuspendLayout();
            // 
            // textBox1
            // 
            this.textBox1.Enabled = false;
            this.textBox1.Location = new System.Drawing.Point(39, 8);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(159, 21);
            this.textBox1.TabIndex = 0;
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(3, 8);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(33, 20);
            this.label1.Text = "RFID";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(201, 8);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(31, 20);
            this.button1.TabIndex = 2;
            this.button1.Text = "Get";
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // pictureBox1
            // 
            this.pictureBox1.Location = new System.Drawing.Point(6, 31);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(228, 152);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(240, 240);
            this.tabControl1.TabIndex = 4;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.button3);
            this.tabPage1.Controls.Add(this.button2);
            this.tabPage1.Controls.Add(this.textBox1);
            this.tabPage1.Controls.Add(this.pictureBox1);
            this.tabPage1.Controls.Add(this.label1);
            this.tabPage1.Controls.Add(this.button1);
            this.tabPage1.Location = new System.Drawing.Point(0, 0);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Size = new System.Drawing.Size(240, 217);
            this.tabPage1.Text = "Entry";
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(145, 189);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(95, 25);
            this.button3.TabIndex = 4;
            this.button3.Text = "Submit";
            this.button3.Click += new System.EventHandler(this.menuItem2_Click);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(7, 189);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(95, 15);
            this.button2.TabIndex = 4;
            this.button2.Text = "Acquire Photo";
            this.button2.Click += new System.EventHandler(this.menuItem1_Click);
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.patientVitalsListView);
            this.tabPage2.Controls.Add(this.DOBLabel);
            this.tabPage2.Controls.Add(this.patientNameLabel);
            this.tabPage2.Controls.Add(this.medPB);
            this.tabPage2.Controls.Add(this.patientPhotoPB);
            this.tabPage2.Controls.Add(this.readMedButton);
            this.tabPage2.Controls.Add(this.readPatientButton);
            this.tabPage2.Location = new System.Drawing.Point(0, 0);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Size = new System.Drawing.Size(232, 214);
            this.tabPage2.Text = "Nurse";
            // 
            // patientVitalsListView
            // 
            this.patientVitalsListView.Columns.Add(this.columnHeader1);
            this.patientVitalsListView.Columns.Add(this.columnHeader2);
            this.patientVitalsListView.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.patientVitalsListView.FullRowSelect = true;
            this.patientVitalsListView.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.patientVitalsListView.Location = new System.Drawing.Point(7, 29);
            this.patientVitalsListView.Name = "patientVitalsListView";
            this.patientVitalsListView.Size = new System.Drawing.Size(108, 183);
            this.patientVitalsListView.TabIndex = 4;
            this.patientVitalsListView.View = System.Windows.Forms.View.Details;
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "Time";
            this.columnHeader1.Width = 57;
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "Temp";
            this.columnHeader2.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.columnHeader2.Width = 40;
            // 
            // DOBLabel
            // 
            this.DOBLabel.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.DOBLabel.Location = new System.Drawing.Point(121, 122);
            this.DOBLabel.Name = "DOBLabel";
            this.DOBLabel.Size = new System.Drawing.Size(100, 15);
            this.DOBLabel.Text = "label2";
            // 
            // patientNameLabel
            // 
            this.patientNameLabel.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.patientNameLabel.Location = new System.Drawing.Point(121, 97);
            this.patientNameLabel.Name = "patientNameLabel";
            this.patientNameLabel.Size = new System.Drawing.Size(116, 28);
            this.patientNameLabel.Text = "label2";
            // 
            // medPB
            // 
            this.medPB.Location = new System.Drawing.Point(121, 140);
            this.medPB.Name = "medPB";
            this.medPB.Size = new System.Drawing.Size(116, 75);
            // 
            // patientPhotoPB
            // 
            this.patientPhotoPB.Location = new System.Drawing.Point(121, 7);
            this.patientPhotoPB.Name = "patientPhotoPB";
            this.patientPhotoPB.Size = new System.Drawing.Size(116, 87);
            this.patientPhotoPB.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
            // 
            // readMedButton
            // 
            this.readMedButton.Font = new System.Drawing.Font("Tahoma", 7F, System.Drawing.FontStyle.Bold);
            this.readMedButton.Location = new System.Drawing.Point(77, 3);
            this.readMedButton.Name = "readMedButton";
            this.readMedButton.Size = new System.Drawing.Size(38, 20);
            this.readMedButton.TabIndex = 0;
            this.readMedButton.Text = "Med";
            this.readMedButton.Click += new System.EventHandler(this.readMedIDClick);
            // 
            // readPatientButton
            // 
            this.readPatientButton.Font = new System.Drawing.Font("Tahoma", 7F, System.Drawing.FontStyle.Bold);
            this.readPatientButton.Location = new System.Drawing.Point(7, 3);
            this.readPatientButton.Name = "readPatientButton";
            this.readPatientButton.Size = new System.Drawing.Size(38, 20);
            this.readPatientButton.TabIndex = 0;
            this.readPatientButton.Text = "Read";
            this.readPatientButton.Click += new System.EventHandler(this.readPatientTagClick);
            // 
            // mobileMedDemo
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(240, 240);
            this.Controls.Add(this.tabControl1);
            this.Location = new System.Drawing.Point(0, 0);
            this.MinimizeBox = false;
            this.Name = "mobileMedDemo";
            this.Text = "COREmobileMedDemo";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage2.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.PictureBox patientPhotoPB;
        private System.Windows.Forms.Button readPatientButton;
        private System.Windows.Forms.Label patientNameLabel;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        protected internal System.Windows.Forms.ListView patientVitalsListView;
        private System.Windows.Forms.Label DOBLabel;
        private System.Windows.Forms.PictureBox medPB;
        private System.Windows.Forms.Button readMedButton;
    }
}

