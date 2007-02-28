namespace GenTag_Server
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
            this.label1 = new System.Windows.Forms.Label();
            this.usernameBox = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.passwordBox = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.dataSourceBox = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.clearLogButton = new System.Windows.Forms.Button();
            this.logLimitSize = new System.Windows.Forms.NumericUpDown();
            this.idBox = new System.Windows.Forms.TextBox();
            this.interactionBox = new System.Windows.Forms.TextBox();
            this.descriptionBox = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.browseButton = new System.Windows.Forms.Button();
            this.saveButton = new System.Windows.Forms.Button();
            this.closeButton = new System.Windows.Forms.Button();
            this.pictImg = new System.Windows.Forms.PictureBox();
            this.clearButton = new System.Windows.Forms.Button();
            this.loadButton = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.interactionID = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.drugDescriptionBox = new System.Windows.Forms.TextBox();
            this.drugIDBox = new System.Windows.Forms.TextBox();
            this.drugLoadButton = new System.Windows.Forms.Button();
            this.label10 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.drugClearButton = new System.Windows.Forms.Button();
            this.drugSaveButton = new System.Windows.Forms.Button();
            this.drugPictureBox = new System.Windows.Forms.PictureBox();
            this.drugBrowseButton = new System.Windows.Forms.Button();
            this.interactionLoad = new System.Windows.Forms.Button();
            this.interactionSave = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.interactionListBox = new System.Windows.Forms.ListBox();
            this.label12 = new System.Windows.Forms.Label();
            this.drugIDAddBox = new System.Windows.Forms.TextBox();
            this.button2 = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.logLimitSize)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictImg)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.drugPictureBox)).BeginInit();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(0, 25);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(36, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Login:";
            // 
            // usernameBox
            // 
            this.usernameBox.Location = new System.Drawing.Point(42, 21);
            this.usernameBox.Name = "usernameBox";
            this.usernameBox.Size = new System.Drawing.Size(100, 20);
            this.usernameBox.TabIndex = 1;
            this.usernameBox.Text = "rfid";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(148, 25);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(56, 13);
            this.label2.TabIndex = 0;
            this.label2.Text = "Password:";
            // 
            // passwordBox
            // 
            this.passwordBox.Location = new System.Drawing.Point(210, 21);
            this.passwordBox.Name = "passwordBox";
            this.passwordBox.PasswordChar = '*';
            this.passwordBox.Size = new System.Drawing.Size(100, 20);
            this.passwordBox.TabIndex = 1;
            this.passwordBox.Text = "rfid2006";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(316, 25);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(89, 13);
            this.label3.TabIndex = 0;
            this.label3.Text = "TNSNames Entry";
            // 
            // dataSourceBox
            // 
            this.dataSourceBox.Location = new System.Drawing.Point(411, 21);
            this.dataSourceBox.Name = "dataSourceBox";
            this.dataSourceBox.Size = new System.Drawing.Size(100, 20);
            this.dataSourceBox.TabIndex = 1;
            this.dataSourceBox.Text = "linserver";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(598, 25);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(51, 13);
            this.label4.TabIndex = 0;
            this.label4.Text = "Log Size:";
            // 
            // clearLogButton
            // 
            this.clearLogButton.Location = new System.Drawing.Point(517, 20);
            this.clearLogButton.Name = "clearLogButton";
            this.clearLogButton.Size = new System.Drawing.Size(75, 23);
            this.clearLogButton.TabIndex = 3;
            this.clearLogButton.Text = "Clear Log";
            this.clearLogButton.UseVisualStyleBackColor = true;
            // 
            // logLimitSize
            // 
            this.logLimitSize.Location = new System.Drawing.Point(655, 21);
            this.logLimitSize.Maximum = new decimal(new int[] {
            2048,
            0,
            0,
            0});
            this.logLimitSize.Minimum = new decimal(new int[] {
            256,
            0,
            0,
            0});
            this.logLimitSize.Name = "logLimitSize";
            this.logLimitSize.Size = new System.Drawing.Size(61, 20);
            this.logLimitSize.TabIndex = 4;
            this.logLimitSize.Value = new decimal(new int[] {
            256,
            0,
            0,
            0});
            // 
            // idBox
            // 
            this.idBox.Location = new System.Drawing.Point(110, 19);
            this.idBox.Name = "idBox";
            this.idBox.Size = new System.Drawing.Size(226, 20);
            this.idBox.TabIndex = 5;
            // 
            // interactionBox
            // 
            this.interactionBox.Location = new System.Drawing.Point(110, 45);
            this.interactionBox.Name = "interactionBox";
            this.interactionBox.Size = new System.Drawing.Size(226, 20);
            this.interactionBox.TabIndex = 5;
            // 
            // descriptionBox
            // 
            this.descriptionBox.Location = new System.Drawing.Point(110, 71);
            this.descriptionBox.Multiline = true;
            this.descriptionBox.Name = "descriptionBox";
            this.descriptionBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.descriptionBox.Size = new System.Drawing.Size(263, 56);
            this.descriptionBox.TabIndex = 5;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(6, 19);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(18, 13);
            this.label5.TabIndex = 6;
            this.label5.Text = "ID";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(6, 48);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(62, 13);
            this.label6.TabIndex = 6;
            this.label6.Text = "Interactions";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(6, 74);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(60, 13);
            this.label7.TabIndex = 6;
            this.label7.Text = "Description";
            // 
            // browseButton
            // 
            this.browseButton.Location = new System.Drawing.Point(6, 145);
            this.browseButton.Name = "browseButton";
            this.browseButton.Size = new System.Drawing.Size(75, 23);
            this.browseButton.TabIndex = 7;
            this.browseButton.Text = "Browse";
            this.browseButton.UseVisualStyleBackColor = true;
            this.browseButton.Click += new System.EventHandler(this.browseButton_Click);
            // 
            // saveButton
            // 
            this.saveButton.Location = new System.Drawing.Point(6, 203);
            this.saveButton.Name = "saveButton";
            this.saveButton.Size = new System.Drawing.Size(75, 23);
            this.saveButton.TabIndex = 7;
            this.saveButton.Text = "Save";
            this.saveButton.UseVisualStyleBackColor = true;
            this.saveButton.Click += new System.EventHandler(this.saveButton_Click);
            // 
            // closeButton
            // 
            this.closeButton.Location = new System.Drawing.Point(675, 551);
            this.closeButton.Name = "closeButton";
            this.closeButton.Size = new System.Drawing.Size(75, 23);
            this.closeButton.TabIndex = 7;
            this.closeButton.Text = "Close";
            this.closeButton.UseVisualStyleBackColor = true;
            this.closeButton.Click += new System.EventHandler(this.closeButton_Click);
            // 
            // pictImg
            // 
            this.pictImg.Location = new System.Drawing.Point(110, 145);
            this.pictImg.Name = "pictImg";
            this.pictImg.Size = new System.Drawing.Size(260, 260);
            this.pictImg.TabIndex = 8;
            this.pictImg.TabStop = false;
            // 
            // clearButton
            // 
            this.clearButton.Location = new System.Drawing.Point(6, 174);
            this.clearButton.Name = "clearButton";
            this.clearButton.Size = new System.Drawing.Size(75, 23);
            this.clearButton.TabIndex = 7;
            this.clearButton.Text = "Clear";
            this.clearButton.UseVisualStyleBackColor = true;
            this.clearButton.Click += new System.EventHandler(this.clearButton_Click);
            // 
            // loadButton
            // 
            this.loadButton.Location = new System.Drawing.Point(6, 232);
            this.loadButton.Name = "loadButton";
            this.loadButton.Size = new System.Drawing.Size(75, 23);
            this.loadButton.TabIndex = 7;
            this.loadButton.Text = "Load";
            this.loadButton.UseVisualStyleBackColor = true;
            this.loadButton.Click += new System.EventHandler(this.loadButton_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.browseButton);
            this.groupBox1.Controls.Add(this.pictImg);
            this.groupBox1.Controls.Add(this.saveButton);
            this.groupBox1.Controls.Add(this.clearButton);
            this.groupBox1.Controls.Add(this.label6);
            this.groupBox1.Controls.Add(this.label5);
            this.groupBox1.Controls.Add(this.label7);
            this.groupBox1.Controls.Add(this.interactionBox);
            this.groupBox1.Controls.Add(this.loadButton);
            this.groupBox1.Controls.Add(this.idBox);
            this.groupBox1.Controls.Add(this.descriptionBox);
            this.groupBox1.Location = new System.Drawing.Point(3, 47);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(382, 419);
            this.groupBox1.TabIndex = 9;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Patient Data";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.drugBrowseButton);
            this.groupBox2.Controls.Add(this.drugPictureBox);
            this.groupBox2.Controls.Add(this.drugDescriptionBox);
            this.groupBox2.Controls.Add(this.drugSaveButton);
            this.groupBox2.Controls.Add(this.drugIDBox);
            this.groupBox2.Controls.Add(this.drugClearButton);
            this.groupBox2.Controls.Add(this.drugLoadButton);
            this.groupBox2.Controls.Add(this.label10);
            this.groupBox2.Controls.Add(this.label11);
            this.groupBox2.Location = new System.Drawing.Point(391, 48);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(359, 418);
            this.groupBox2.TabIndex = 10;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Drug Data";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.interactionListBox);
            this.groupBox3.Controls.Add(this.drugIDAddBox);
            this.groupBox3.Controls.Add(this.label12);
            this.groupBox3.Controls.Add(this.interactionID);
            this.groupBox3.Controls.Add(this.label8);
            this.groupBox3.Controls.Add(this.interactionSave);
            this.groupBox3.Controls.Add(this.label9);
            this.groupBox3.Controls.Add(this.button2);
            this.groupBox3.Controls.Add(this.button1);
            this.groupBox3.Controls.Add(this.interactionLoad);
            this.groupBox3.Location = new System.Drawing.Point(3, 472);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(666, 102);
            this.groupBox3.TabIndex = 10;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Interactions";
            // 
            // interactionID
            // 
            this.interactionID.Location = new System.Drawing.Point(56, 16);
            this.interactionID.Name = "interactionID";
            this.interactionID.Size = new System.Drawing.Size(226, 20);
            this.interactionID.TabIndex = 5;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(6, 19);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(18, 13);
            this.label8.TabIndex = 6;
            this.label8.Text = "ID";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(6, 48);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(44, 13);
            this.label9.TabIndex = 6;
            this.label9.Text = "Drug ID";
            // 
            // drugDescriptionBox
            // 
            this.drugDescriptionBox.Location = new System.Drawing.Point(88, 70);
            this.drugDescriptionBox.Multiline = true;
            this.drugDescriptionBox.Name = "drugDescriptionBox";
            this.drugDescriptionBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.drugDescriptionBox.Size = new System.Drawing.Size(263, 56);
            this.drugDescriptionBox.TabIndex = 5;
            // 
            // drugIDBox
            // 
            this.drugIDBox.Location = new System.Drawing.Point(88, 19);
            this.drugIDBox.Name = "drugIDBox";
            this.drugIDBox.Size = new System.Drawing.Size(226, 20);
            this.drugIDBox.TabIndex = 5;
            // 
            // drugLoadButton
            // 
            this.drugLoadButton.Location = new System.Drawing.Point(7, 231);
            this.drugLoadButton.Name = "drugLoadButton";
            this.drugLoadButton.Size = new System.Drawing.Size(75, 23);
            this.drugLoadButton.TabIndex = 7;
            this.drugLoadButton.Text = "Load";
            this.drugLoadButton.UseVisualStyleBackColor = true;
            this.drugLoadButton.Click += new System.EventHandler(this.drugLoadButton_Click);
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(7, 73);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(60, 13);
            this.label10.TabIndex = 6;
            this.label10.Text = "Description";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(7, 22);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(18, 13);
            this.label11.TabIndex = 6;
            this.label11.Text = "ID";
            // 
            // drugClearButton
            // 
            this.drugClearButton.Location = new System.Drawing.Point(7, 173);
            this.drugClearButton.Name = "drugClearButton";
            this.drugClearButton.Size = new System.Drawing.Size(75, 23);
            this.drugClearButton.TabIndex = 7;
            this.drugClearButton.Text = "Clear";
            this.drugClearButton.UseVisualStyleBackColor = true;
            this.drugClearButton.Click += new System.EventHandler(this.drugClearButton_Click);
            // 
            // drugSaveButton
            // 
            this.drugSaveButton.Location = new System.Drawing.Point(7, 202);
            this.drugSaveButton.Name = "drugSaveButton";
            this.drugSaveButton.Size = new System.Drawing.Size(75, 23);
            this.drugSaveButton.TabIndex = 7;
            this.drugSaveButton.Text = "Save";
            this.drugSaveButton.UseVisualStyleBackColor = true;
            this.drugSaveButton.Click += new System.EventHandler(this.drugSaveButton_Click);
            // 
            // drugPictureBox
            // 
            this.drugPictureBox.Location = new System.Drawing.Point(88, 144);
            this.drugPictureBox.Name = "drugPictureBox";
            this.drugPictureBox.Size = new System.Drawing.Size(260, 260);
            this.drugPictureBox.TabIndex = 8;
            this.drugPictureBox.TabStop = false;
            // 
            // drugBrowseButton
            // 
            this.drugBrowseButton.Location = new System.Drawing.Point(7, 144);
            this.drugBrowseButton.Name = "drugBrowseButton";
            this.drugBrowseButton.Size = new System.Drawing.Size(75, 23);
            this.drugBrowseButton.TabIndex = 7;
            this.drugBrowseButton.Text = "Browse";
            this.drugBrowseButton.UseVisualStyleBackColor = true;
            this.drugBrowseButton.Click += new System.EventHandler(this.drugBrowseButton_Click);
            // 
            // interactionLoad
            // 
            this.interactionLoad.Location = new System.Drawing.Point(585, 43);
            this.interactionLoad.Name = "interactionLoad";
            this.interactionLoad.Size = new System.Drawing.Size(75, 23);
            this.interactionLoad.TabIndex = 7;
            this.interactionLoad.Text = "Load";
            this.interactionLoad.UseVisualStyleBackColor = true;
            this.interactionLoad.Click += new System.EventHandler(this.interactionLoad_Click);
            // 
            // interactionSave
            // 
            this.interactionSave.Location = new System.Drawing.Point(585, 14);
            this.interactionSave.Name = "interactionSave";
            this.interactionSave.Size = new System.Drawing.Size(75, 23);
            this.interactionSave.TabIndex = 7;
            this.interactionSave.Text = "Save";
            this.interactionSave.UseVisualStyleBackColor = true;
            this.interactionSave.Click += new System.EventHandler(this.interactionSave_Click);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(585, 72);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 7;
            this.button1.Text = "Add";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // interactionListBox
            // 
            this.interactionListBox.FormattingEnabled = true;
            this.interactionListBox.Location = new System.Drawing.Point(56, 48);
            this.interactionListBox.Name = "interactionListBox";
            this.interactionListBox.Size = new System.Drawing.Size(145, 43);
            this.interactionListBox.TabIndex = 8;
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(400, 77);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(44, 13);
            this.label12.TabIndex = 6;
            this.label12.Text = "Drug ID";
            // 
            // drugIDAddBox
            // 
            this.drugIDAddBox.Location = new System.Drawing.Point(450, 74);
            this.drugIDAddBox.Name = "drugIDAddBox";
            this.drugIDAddBox.Size = new System.Drawing.Size(129, 20);
            this.drugIDAddBox.TabIndex = 5;
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(207, 68);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(75, 23);
            this.button2.TabIndex = 7;
            this.button2.Text = "Clear";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // mainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(762, 586);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.closeButton);
            this.Controls.Add(this.logLimitSize);
            this.Controls.Add(this.clearLogButton);
            this.Controls.Add(this.dataSourceBox);
            this.Controls.Add(this.passwordBox);
            this.Controls.Add(this.usernameBox);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "mainForm";
            this.Text = "GenTag Server";
            ((System.ComponentModel.ISupportInitialize)(this.logLimitSize)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictImg)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.drugPictureBox)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox usernameBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox passwordBox;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox dataSourceBox;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button clearLogButton;
        private System.Windows.Forms.NumericUpDown logLimitSize;
        private System.Windows.Forms.TextBox idBox;
        private System.Windows.Forms.TextBox interactionBox;
        private System.Windows.Forms.TextBox descriptionBox;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Button browseButton;
        private System.Windows.Forms.Button saveButton;
        private System.Windows.Forms.Button closeButton;
        private System.Windows.Forms.PictureBox pictImg;
        private System.Windows.Forms.Button clearButton;
        private System.Windows.Forms.Button loadButton;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Button drugBrowseButton;
        private System.Windows.Forms.PictureBox drugPictureBox;
        private System.Windows.Forms.TextBox drugDescriptionBox;
        private System.Windows.Forms.Button drugSaveButton;
        private System.Windows.Forms.TextBox drugIDBox;
        private System.Windows.Forms.Button drugClearButton;
        private System.Windows.Forms.Button drugLoadButton;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.TextBox interactionID;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Button interactionSave;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Button interactionLoad;
        private System.Windows.Forms.ListBox interactionListBox;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.TextBox drugIDAddBox;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Button button2;
    }
}

