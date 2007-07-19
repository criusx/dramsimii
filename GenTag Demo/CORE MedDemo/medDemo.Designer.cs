namespace CORE_MedDemo
{
    partial class medDemo
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
            System.Windows.Forms.Label label1;
            System.Windows.Forms.Label label2;
            System.Windows.Forms.Label label4;
            System.Windows.Forms.PictureBox pictureBox1;
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(medDemo));
            System.Windows.Forms.Label statusLabel;
            this.firstNameTextBox = new System.Windows.Forms.TextBox();
            this.dateOfBirthDateTimePicker = new System.Windows.Forms.DateTimePicker();
            this.braceletIDtextBox = new System.Windows.Forms.TextBox();
            this.currentMedsListBox = new System.Windows.Forms.ListBox();
            this.getIDButton = new System.Windows.Forms.Button();
            this.submitButton = new System.Windows.Forms.Button();
            this.currentMedEntryBox = new System.Windows.Forms.TextBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.addMedButton = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.button1 = new System.Windows.Forms.Button();
            this.allergyEntryBox = new System.Windows.Forms.TextBox();
            this.allergiesListBox = new System.Windows.Forms.ListBox();
            this.middleNameTextBox = new System.Windows.Forms.TextBox();
            this.lastNameTextBox = new System.Windows.Forms.TextBox();
            label1 = new System.Windows.Forms.Label();
            label2 = new System.Windows.Forms.Label();
            label4 = new System.Windows.Forms.Label();
            pictureBox1 = new System.Windows.Forms.PictureBox();
            statusLabel = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(pictureBox1)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new System.Drawing.Point(38, 37);
            label1.Name = "label1";
            label1.Size = new System.Drawing.Size(35, 13);
            label1.TabIndex = 2;
            label1.Text = "Name";
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new System.Drawing.Point(7, 66);
            label2.Name = "label2";
            label2.Size = new System.Drawing.Size(66, 13);
            label2.TabIndex = 2;
            label2.Text = "Date of Birth";
            // 
            // label4
            // 
            label4.AutoSize = true;
            label4.Location = new System.Drawing.Point(13, 11);
            label4.Name = "label4";
            label4.Size = new System.Drawing.Size(60, 13);
            label4.TabIndex = 2;
            label4.Text = "Bracelet ID";
            // 
            // pictureBox1
            // 
            pictureBox1.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("pictureBox1.BackgroundImage")));
            pictureBox1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            pictureBox1.InitialImage = null;
            pictureBox1.Location = new System.Drawing.Point(-7, 445);
            pictureBox1.Name = "pictureBox1";
            pictureBox1.Size = new System.Drawing.Size(424, 98);
            pictureBox1.TabIndex = 5;
            pictureBox1.TabStop = false;
            // 
            // statusLabel
            // 
            statusLabel.AutoSize = true;
            statusLabel.Location = new System.Drawing.Point(13, 289);
            statusLabel.Name = "statusLabel";
            statusLabel.Size = new System.Drawing.Size(0, 13);
            statusLabel.TabIndex = 2;
            // 
            // firstNameTextBox
            // 
            this.firstNameTextBox.Location = new System.Drawing.Point(79, 34);
            this.firstNameTextBox.Name = "firstNameTextBox";
            this.firstNameTextBox.Size = new System.Drawing.Size(84, 20);
            this.firstNameTextBox.TabIndex = 1;
            // 
            // dateOfBirthDateTimePicker
            // 
            this.dateOfBirthDateTimePicker.Format = System.Windows.Forms.DateTimePickerFormat.Short;
            this.dateOfBirthDateTimePicker.Location = new System.Drawing.Point(79, 60);
            this.dateOfBirthDateTimePicker.Name = "dateOfBirthDateTimePicker";
            this.dateOfBirthDateTimePicker.Size = new System.Drawing.Size(84, 20);
            this.dateOfBirthDateTimePicker.TabIndex = 4;
            // 
            // braceletIDtextBox
            // 
            this.braceletIDtextBox.Location = new System.Drawing.Point(79, 8);
            this.braceletIDtextBox.Name = "braceletIDtextBox";
            this.braceletIDtextBox.Size = new System.Drawing.Size(269, 20);
            this.braceletIDtextBox.TabIndex = 0;
            // 
            // currentMedsListBox
            // 
            this.currentMedsListBox.FormattingEnabled = true;
            this.currentMedsListBox.Location = new System.Drawing.Point(69, 45);
            this.currentMedsListBox.Name = "currentMedsListBox";
            this.currentMedsListBox.Size = new System.Drawing.Size(200, 95);
            this.currentMedsListBox.TabIndex = 3;
            this.currentMedsListBox.DoubleClick += new System.EventHandler(this.ListBox_DoubleClick);
            // 
            // getIDButton
            // 
            this.getIDButton.Location = new System.Drawing.Point(354, 6);
            this.getIDButton.Name = "getIDButton";
            this.getIDButton.Size = new System.Drawing.Size(46, 23);
            this.getIDButton.TabIndex = 4;
            this.getIDButton.Text = "Get ID";
            this.getIDButton.UseVisualStyleBackColor = true;
            this.getIDButton.Click += new System.EventHandler(this.getIDButton_Click);
            // 
            // submitButton
            // 
            this.submitButton.Location = new System.Drawing.Point(309, 456);
            this.submitButton.Name = "submitButton";
            this.submitButton.Size = new System.Drawing.Size(75, 23);
            this.submitButton.TabIndex = 6;
            this.submitButton.Text = "Submit";
            this.submitButton.UseVisualStyleBackColor = true;
            this.submitButton.Click += new System.EventHandler(this.submitButton_Click);
            // 
            // currentMedEntryBox
            // 
            this.currentMedEntryBox.Location = new System.Drawing.Point(69, 19);
            this.currentMedEntryBox.Name = "currentMedEntryBox";
            this.currentMedEntryBox.Size = new System.Drawing.Size(200, 20);
            this.currentMedEntryBox.TabIndex = 6;
            this.currentMedEntryBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.EntryBox_KeyDown);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.addMedButton);
            this.groupBox1.Controls.Add(this.currentMedEntryBox);
            this.groupBox1.Controls.Add(this.currentMedsListBox);
            this.groupBox1.Location = new System.Drawing.Point(10, 249);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(338, 157);
            this.groupBox1.TabIndex = 7;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Current Medications";
            // 
            // addMedButton
            // 
            this.addMedButton.Location = new System.Drawing.Point(276, 15);
            this.addMedButton.Name = "addMedButton";
            this.addMedButton.Size = new System.Drawing.Size(40, 23);
            this.addMedButton.TabIndex = 4;
            this.addMedButton.Text = "Add";
            this.addMedButton.UseVisualStyleBackColor = true;
            this.addMedButton.Click += new System.EventHandler(this.button1_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.button1);
            this.groupBox2.Controls.Add(this.allergyEntryBox);
            this.groupBox2.Controls.Add(this.allergiesListBox);
            this.groupBox2.Location = new System.Drawing.Point(10, 86);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(338, 157);
            this.groupBox2.TabIndex = 7;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Allergies";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(276, 15);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(40, 23);
            this.button1.TabIndex = 4;
            this.button1.Text = "Add";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // allergyEntryBox
            // 
            this.allergyEntryBox.Location = new System.Drawing.Point(69, 19);
            this.allergyEntryBox.Name = "allergyEntryBox";
            this.allergyEntryBox.Size = new System.Drawing.Size(200, 20);
            this.allergyEntryBox.TabIndex = 5;
            this.allergyEntryBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.EntryBox_KeyDown);
            // 
            // allergiesListBox
            // 
            this.allergiesListBox.FormattingEnabled = true;
            this.allergiesListBox.Location = new System.Drawing.Point(69, 45);
            this.allergiesListBox.Name = "allergiesListBox";
            this.allergiesListBox.Size = new System.Drawing.Size(200, 95);
            this.allergiesListBox.TabIndex = 3;
            this.allergiesListBox.DoubleClick += new System.EventHandler(this.ListBox_DoubleClick);
            // 
            // middleNameTextBox
            // 
            this.middleNameTextBox.Location = new System.Drawing.Point(169, 34);
            this.middleNameTextBox.Name = "middleNameTextBox";
            this.middleNameTextBox.Size = new System.Drawing.Size(56, 20);
            this.middleNameTextBox.TabIndex = 2;
            // 
            // lastNameTextBox
            // 
            this.lastNameTextBox.Location = new System.Drawing.Point(231, 34);
            this.lastNameTextBox.Name = "lastNameTextBox";
            this.lastNameTextBox.Size = new System.Drawing.Size(117, 20);
            this.lastNameTextBox.TabIndex = 3;
            // 
            // medDemo
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(408, 538);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.submitButton);
            this.Controls.Add(pictureBox1);
            this.Controls.Add(this.getIDButton);
            this.Controls.Add(statusLabel);
            this.Controls.Add(label4);
            this.Controls.Add(label2);
            this.Controls.Add(label1);
            this.Controls.Add(this.dateOfBirthDateTimePicker);
            this.Controls.Add(this.braceletIDtextBox);
            this.Controls.Add(this.lastNameTextBox);
            this.Controls.Add(this.middleNameTextBox);
            this.Controls.Add(this.firstNameTextBox);
            this.MaximizeBox = false;
            this.Name = "medDemo";
            this.Text = "Patient Creation Client";
            ((System.ComponentModel.ISupportInitialize)(pictureBox1)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox firstNameTextBox;
        private System.Windows.Forms.DateTimePicker dateOfBirthDateTimePicker;
        private System.Windows.Forms.TextBox braceletIDtextBox;
        private System.Windows.Forms.ListBox currentMedsListBox;
        private System.Windows.Forms.Button getIDButton;
        private System.Windows.Forms.Button submitButton;
        private System.Windows.Forms.TextBox currentMedEntryBox;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button addMedButton;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.TextBox allergyEntryBox;
        private System.Windows.Forms.ListBox allergiesListBox;
        private System.Windows.Forms.TextBox middleNameTextBox;
        private System.Windows.Forms.TextBox lastNameTextBox;
    }
}

