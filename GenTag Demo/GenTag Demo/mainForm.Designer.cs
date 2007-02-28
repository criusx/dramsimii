namespace GenTag_Demo
{
    partial class mainForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(mainForm));
            this.readIDButton = new System.Windows.Forms.Button();
            this.checkBox1 = new System.Windows.Forms.CheckBox();
            this.readLogButton = new System.Windows.Forms.Button();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.treeView1 = new System.Windows.Forms.TreeView();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.textBox9 = new System.Windows.Forms.TextBox();
            this.listBox1 = new System.Windows.Forms.ListBox();
            this.checkBox2 = new System.Windows.Forms.CheckBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.textBox3 = new System.Windows.Forms.TextBox();
            this.textBox2 = new System.Windows.Forms.TextBox();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.treeView2 = new System.Windows.Forms.TreeView();
            this.manualIDButton = new System.Windows.Forms.Button();
            this.textBox4 = new System.Windows.Forms.TextBox();
            this.tabPage4 = new System.Windows.Forms.TabPage();
            this.comboBox1 = new System.Windows.Forms.ComboBox();
            this.readValueButton = new System.Windows.Forms.Button();
            this.setValueButton = new System.Windows.Forms.Button();
            this.textBox5 = new System.Windows.Forms.TextBox();
            this.textBox6 = new System.Windows.Forms.TextBox();
            this.textBox8 = new System.Windows.Forms.TextBox();
            this.textBox7 = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.tabPage5 = new System.Windows.Forms.TabPage();
            this.tabPage6 = new System.Windows.Forms.TabPage();
            this.patientNameBox = new System.Windows.Forms.TextBox();
            this.patientPhoto = new System.Windows.Forms.PictureBox();
            this.label10 = new System.Windows.Forms.Label();
            this.patientDescriptionBox = new System.Windows.Forms.TextBox();
            this.label11 = new System.Windows.Forms.Label();
            this.readPatientButton = new System.Windows.Forms.Button();
            this.medicationButton = new System.Windows.Forms.Button();
            this.drugPhoto = new System.Windows.Forms.PictureBox();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.tabPage3.SuspendLayout();
            this.tabPage4.SuspendLayout();
            this.tabPage5.SuspendLayout();
            this.SuspendLayout();
            // 
            // readIDButton
            // 
            this.readIDButton.Location = new System.Drawing.Point(7, 187);
            this.readIDButton.Name = "readIDButton";
            this.readIDButton.Size = new System.Drawing.Size(71, 27);
            this.readIDButton.TabIndex = 0;
            this.readIDButton.Click += new System.EventHandler(this.readIDClick);
            // 
            // checkBox1
            // 
            this.checkBox1.Enabled = false;
            this.checkBox1.Location = new System.Drawing.Point(53, 163);
            this.checkBox1.Name = "checkBox1";
            this.checkBox1.Size = new System.Drawing.Size(25, 18);
            this.checkBox1.TabIndex = 4;
            // 
            // readLogButton
            // 
            this.readLogButton.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.readLogButton.Location = new System.Drawing.Point(2, 196);
            this.readLogButton.Name = "readLogButton";
            this.readLogButton.Size = new System.Drawing.Size(84, 18);
            this.readLogButton.TabIndex = 0;
            this.readLogButton.Click += new System.EventHandler(this.readLogClick);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Controls.Add(this.tabPage3);
            this.tabControl1.Controls.Add(this.tabPage4);
            this.tabControl1.Controls.Add(this.tabPage5);
            this.tabControl1.Controls.Add(this.tabPage6);
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(240, 240);
            this.tabControl1.TabIndex = 5;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.pictureBox1);
            this.tabPage1.Controls.Add(this.readIDButton);
            this.tabPage1.Controls.Add(this.checkBox1);
            this.tabPage1.Controls.Add(this.treeView1);
            this.tabPage1.Location = new System.Drawing.Point(0, 0);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Size = new System.Drawing.Size(240, 217);
            this.tabPage1.Text = "tabPage1";
            // 
            // pictureBox1
            // 
            this.pictureBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
            this.pictureBox1.Location = new System.Drawing.Point(79, 125);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(154, 90);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
            // 
            // treeView1
            // 
            this.treeView1.Location = new System.Drawing.Point(0, 0);
            this.treeView1.Name = "treeView1";
            this.treeView1.Size = new System.Drawing.Size(240, 121);
            this.treeView1.TabIndex = 2;
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.textBox9);
            this.tabPage2.Controls.Add(this.listBox1);
            this.tabPage2.Controls.Add(this.checkBox2);
            this.tabPage2.Controls.Add(this.label4);
            this.tabPage2.Controls.Add(this.label3);
            this.tabPage2.Controls.Add(this.label2);
            this.tabPage2.Controls.Add(this.label1);
            this.tabPage2.Controls.Add(this.textBox3);
            this.tabPage2.Controls.Add(this.textBox2);
            this.tabPage2.Controls.Add(this.textBox1);
            this.tabPage2.Controls.Add(this.readLogButton);
            this.tabPage2.Location = new System.Drawing.Point(0, 0);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Size = new System.Drawing.Size(232, 214);
            this.tabPage2.Text = "tabPage2";
            // 
            // textBox9
            // 
            this.textBox9.Location = new System.Drawing.Point(10, 109);
            this.textBox9.Multiline = true;
            this.textBox9.Name = "textBox9";
            this.textBox9.ReadOnly = true;
            this.textBox9.Size = new System.Drawing.Size(76, 57);
            this.textBox9.TabIndex = 11;
            this.textBox9.Text = "textBox9";
            // 
            // listBox1
            // 
            this.listBox1.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.listBox1.Location = new System.Drawing.Point(93, 27);
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new System.Drawing.Size(144, 184);
            this.listBox1.TabIndex = 6;
            // 
            // checkBox2
            // 
            this.checkBox2.Enabled = false;
            this.checkBox2.Location = new System.Drawing.Point(62, 172);
            this.checkBox2.Name = "checkBox2";
            this.checkBox2.Size = new System.Drawing.Size(25, 18);
            this.checkBox2.TabIndex = 4;
            // 
            // label4
            // 
            this.label4.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label4.Location = new System.Drawing.Point(93, 4);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(144, 20);
            this.label4.Tag = "";
            this.label4.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(2, 78);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(50, 20);
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(2, 51);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(50, 20);
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(2, 24);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(50, 20);
            // 
            // textBox3
            // 
            this.textBox3.Location = new System.Drawing.Point(57, 78);
            this.textBox3.Name = "textBox3";
            this.textBox3.ReadOnly = true;
            this.textBox3.Size = new System.Drawing.Size(32, 21);
            this.textBox3.TabIndex = 1;
            // 
            // textBox2
            // 
            this.textBox2.Location = new System.Drawing.Point(57, 51);
            this.textBox2.MaxLength = 3;
            this.textBox2.Name = "textBox2";
            this.textBox2.ReadOnly = true;
            this.textBox2.Size = new System.Drawing.Size(32, 21);
            this.textBox2.TabIndex = 1;
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(57, 24);
            this.textBox1.MaxLength = 3;
            this.textBox1.Name = "textBox1";
            this.textBox1.ReadOnly = true;
            this.textBox1.Size = new System.Drawing.Size(32, 21);
            this.textBox1.TabIndex = 1;
            // 
            // tabPage3
            // 
            this.tabPage3.Controls.Add(this.treeView2);
            this.tabPage3.Controls.Add(this.manualIDButton);
            this.tabPage3.Controls.Add(this.textBox4);
            this.tabPage3.Location = new System.Drawing.Point(0, 0);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Size = new System.Drawing.Size(232, 214);
            this.tabPage3.Text = "tabPage3";
            // 
            // treeView2
            // 
            this.treeView2.Location = new System.Drawing.Point(0, 0);
            this.treeView2.Name = "treeView2";
            this.treeView2.Size = new System.Drawing.Size(240, 161);
            this.treeView2.TabIndex = 2;
            // 
            // manualIDButton
            // 
            this.manualIDButton.Location = new System.Drawing.Point(8, 167);
            this.manualIDButton.Name = "manualIDButton";
            this.manualIDButton.Size = new System.Drawing.Size(72, 20);
            this.manualIDButton.TabIndex = 1;
            this.manualIDButton.Click += new System.EventHandler(this.manualLookupClick);
            // 
            // textBox4
            // 
            this.textBox4.Location = new System.Drawing.Point(8, 193);
            this.textBox4.MaxLength = 25;
            this.textBox4.Name = "textBox4";
            this.textBox4.Size = new System.Drawing.Size(224, 21);
            this.textBox4.TabIndex = 0;
            // 
            // tabPage4
            // 
            this.tabPage4.Controls.Add(this.comboBox1);
            this.tabPage4.Controls.Add(this.readValueButton);
            this.tabPage4.Controls.Add(this.setValueButton);
            this.tabPage4.Controls.Add(this.textBox5);
            this.tabPage4.Controls.Add(this.textBox6);
            this.tabPage4.Controls.Add(this.textBox8);
            this.tabPage4.Controls.Add(this.textBox7);
            this.tabPage4.Controls.Add(this.label8);
            this.tabPage4.Controls.Add(this.label7);
            this.tabPage4.Controls.Add(this.label6);
            this.tabPage4.Controls.Add(this.label9);
            this.tabPage4.Controls.Add(this.label5);
            this.tabPage4.Location = new System.Drawing.Point(0, 0);
            this.tabPage4.Name = "tabPage4";
            this.tabPage4.Size = new System.Drawing.Size(232, 214);
            this.tabPage4.Text = "tabPage4";
            // 
            // comboBox1
            // 
            this.comboBox1.Items.Add("All");
            this.comboBox1.Items.Add("Violations");
            this.comboBox1.Location = new System.Drawing.Point(105, 115);
            this.comboBox1.Name = "comboBox1";
            this.comboBox1.Size = new System.Drawing.Size(60, 22);
            this.comboBox1.TabIndex = 8;
            // 
            // readValueButton
            // 
            this.readValueButton.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Regular);
            this.readValueButton.Location = new System.Drawing.Point(161, 194);
            this.readValueButton.Name = "readValueButton";
            this.readValueButton.Size = new System.Drawing.Size(72, 20);
            this.readValueButton.TabIndex = 6;
            this.readValueButton.Text = "button4";
            this.readValueButton.Click += new System.EventHandler(this.button5_Click);
            // 
            // setValueButton
            // 
            this.setValueButton.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Regular);
            this.setValueButton.Location = new System.Drawing.Point(7, 194);
            this.setValueButton.Name = "setValueButton";
            this.setValueButton.Size = new System.Drawing.Size(72, 20);
            this.setValueButton.TabIndex = 6;
            this.setValueButton.Text = "button4";
            this.setValueButton.Click += new System.EventHandler(this.setVSSettingsClick);
            // 
            // textBox5
            // 
            this.textBox5.Location = new System.Drawing.Point(105, 78);
            this.textBox5.Name = "textBox5";
            this.textBox5.Size = new System.Drawing.Size(32, 21);
            this.textBox5.TabIndex = 5;
            // 
            // textBox6
            // 
            this.textBox6.Location = new System.Drawing.Point(105, 41);
            this.textBox6.MaxLength = 3;
            this.textBox6.Name = "textBox6";
            this.textBox6.Size = new System.Drawing.Size(60, 21);
            this.textBox6.TabIndex = 4;
            // 
            // textBox8
            // 
            this.textBox8.Location = new System.Drawing.Point(105, 152);
            this.textBox8.MaxLength = 3;
            this.textBox8.Name = "textBox8";
            this.textBox8.Size = new System.Drawing.Size(32, 21);
            this.textBox8.TabIndex = 3;
            // 
            // textBox7
            // 
            this.textBox7.Location = new System.Drawing.Point(105, 0);
            this.textBox7.MaxLength = 3;
            this.textBox7.Name = "textBox7";
            this.textBox7.Size = new System.Drawing.Size(60, 21);
            this.textBox7.TabIndex = 3;
            // 
            // label8
            // 
            this.label8.Location = new System.Drawing.Point(8, 115);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(100, 20);
            this.label8.Text = "logmode";
            // 
            // label7
            // 
            this.label7.Location = new System.Drawing.Point(8, 78);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(100, 20);
            this.label7.Text = "logperiod";
            // 
            // label6
            // 
            this.label6.Location = new System.Drawing.Point(8, 41);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(100, 20);
            this.label6.Text = "hilimit";
            // 
            // label9
            // 
            this.label9.Location = new System.Drawing.Point(7, 152);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(100, 20);
            this.label9.Text = "batChiItv";
            // 
            // label5
            // 
            this.label5.Location = new System.Drawing.Point(8, 4);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(100, 20);
            this.label5.Text = "lolimit";
            // 
            // tabPage5
            // 
            this.tabPage5.Controls.Add(this.medicationButton);
            this.tabPage5.Controls.Add(this.readPatientButton);
            this.tabPage5.Controls.Add(this.label11);
            this.tabPage5.Controls.Add(this.label10);
            this.tabPage5.Controls.Add(this.drugPhoto);
            this.tabPage5.Controls.Add(this.patientPhoto);
            this.tabPage5.Controls.Add(this.patientDescriptionBox);
            this.tabPage5.Controls.Add(this.patientNameBox);
            this.tabPage5.Location = new System.Drawing.Point(0, 0);
            this.tabPage5.Name = "tabPage5";
            this.tabPage5.Size = new System.Drawing.Size(240, 217);
            this.tabPage5.Text = "Patient";
            // 
            // tabPage6
            // 
            this.tabPage6.Location = new System.Drawing.Point(0, 0);
            this.tabPage6.Name = "tabPage6";
            this.tabPage6.Size = new System.Drawing.Size(240, 217);
            this.tabPage6.Text = "Auth";
            // 
            // patientNameBox
            // 
            this.patientNameBox.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.patientNameBox.Location = new System.Drawing.Point(133, 151);
            this.patientNameBox.Name = "patientNameBox";
            this.patientNameBox.Size = new System.Drawing.Size(100, 19);
            this.patientNameBox.TabIndex = 0;
            // 
            // patientPhoto
            // 
            this.patientPhoto.Location = new System.Drawing.Point(8, 8);
            this.patientPhoto.Name = "patientPhoto";
            this.patientPhoto.Size = new System.Drawing.Size(107, 121);
            this.patientPhoto.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
            // 
            // label10
            // 
            this.label10.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label10.Location = new System.Drawing.Point(133, 135);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(100, 12);
            this.label10.Text = "Patient Name";
            // 
            // patientDescriptionBox
            // 
            this.patientDescriptionBox.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.patientDescriptionBox.Location = new System.Drawing.Point(8, 174);
            this.patientDescriptionBox.Multiline = true;
            this.patientDescriptionBox.Name = "patientDescriptionBox";
            this.patientDescriptionBox.Size = new System.Drawing.Size(225, 40);
            this.patientDescriptionBox.TabIndex = 0;
            // 
            // label11
            // 
            this.label11.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label11.Location = new System.Drawing.Point(7, 158);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(100, 12);
            this.label11.Text = "Patient Description";
            // 
            // readPatientButton
            // 
            this.readPatientButton.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.readPatientButton.Location = new System.Drawing.Point(80, 135);
            this.readPatientButton.Name = "readPatientButton";
            this.readPatientButton.Size = new System.Drawing.Size(48, 20);
            this.readPatientButton.TabIndex = 4;
            this.readPatientButton.Text = "Patient";
            this.readPatientButton.Click += new System.EventHandler(this.readPatientButton_Click);
            // 
            // medicationButton
            // 
            this.medicationButton.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.medicationButton.Location = new System.Drawing.Point(8, 135);
            this.medicationButton.Name = "medicationButton";
            this.medicationButton.Size = new System.Drawing.Size(66, 20);
            this.medicationButton.TabIndex = 4;
            this.medicationButton.Text = "Medication";
            this.medicationButton.Click += new System.EventHandler(this.medicationButton_Click);
            // 
            // drugPhoto
            // 
            this.drugPhoto.Location = new System.Drawing.Point(121, 8);
            this.drugPhoto.Name = "drugPhoto";
            this.drugPhoto.Size = new System.Drawing.Size(112, 121);
            this.drugPhoto.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            // 
            // mainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(240, 320);
            this.Controls.Add(this.tabControl1);
            this.KeyPreview = true;
            this.Location = new System.Drawing.Point(0, 0);
            this.MinimizeBox = false;
            this.Name = "mainForm";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.mainForm_KeyDown);
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage2.ResumeLayout(false);
            this.tabPage3.ResumeLayout(false);
            this.tabPage4.ResumeLayout(false);
            this.tabPage5.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button readIDButton;
        private System.Windows.Forms.CheckBox checkBox1;
        private System.Windows.Forms.Button readLogButton;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox textBox3;
        private System.Windows.Forms.TextBox textBox2;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ListBox listBox1;
        private System.Windows.Forms.CheckBox checkBox2;
        private System.Windows.Forms.TreeView treeView1;
        private System.Windows.Forms.TabPage tabPage3;
        private System.Windows.Forms.TabPage tabPage4;
        private System.Windows.Forms.Button manualIDButton;
        private System.Windows.Forms.TextBox textBox4;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TreeView treeView2;
        private System.Windows.Forms.Button readValueButton;
        private System.Windows.Forms.Button setValueButton;
        private System.Windows.Forms.TextBox textBox5;
        private System.Windows.Forms.TextBox textBox6;
        private System.Windows.Forms.TextBox textBox7;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.ComboBox comboBox1;
        private System.Windows.Forms.TextBox textBox8;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.TextBox textBox9;
        private System.Windows.Forms.TabPage tabPage5;
        private System.Windows.Forms.Button medicationButton;
        private System.Windows.Forms.Button readPatientButton;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.PictureBox patientPhoto;
        private System.Windows.Forms.TextBox patientDescriptionBox;
        private System.Windows.Forms.TextBox patientNameBox;
        private System.Windows.Forms.TabPage tabPage6;
        private System.Windows.Forms.PictureBox drugPhoto;
    }
}

