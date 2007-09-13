namespace GentagDemo
{
    partial class wineClient
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(wineClient));
            this.readIDButton = new System.Windows.Forms.Button();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.authPage = new System.Windows.Forms.TabPage();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.treeView1 = new System.Windows.Forms.TreeView();
            this.winePage = new System.Windows.Forms.TabPage();
            this.wineButton = new System.Windows.Forms.Button();
            this.wineCountryLabel = new System.Windows.Forms.Label();
            this.wineTypeLabel = new System.Windows.Forms.Label();
            this.wineVineyardLabel = new System.Windows.Forms.Label();
            this.wineYearLabel = new System.Windows.Forms.Label();
            this.wineAuthPictureBox = new System.Windows.Forms.PictureBox();
            this.winePictureBox = new System.Windows.Forms.PictureBox();
            this.wineReviewTextBox = new System.Windows.Forms.TextBox();
            this.tabControl1.SuspendLayout();
            this.authPage.SuspendLayout();
            this.winePage.SuspendLayout();
            this.SuspendLayout();
            // 
            // readIDButton
            // 
            this.readIDButton.Location = new System.Drawing.Point(2, 184);
            this.readIDButton.Name = "readIDButton";
            this.readIDButton.Size = new System.Drawing.Size(71, 30);
            this.readIDButton.TabIndex = 0;
            this.readIDButton.Text = "Read Tag";
            this.readIDButton.Click += new System.EventHandler(this.readerClick);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.authPage);
            this.tabControl1.Controls.Add(this.winePage);
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(240, 240);
            this.tabControl1.TabIndex = 5;
            this.tabControl1.SelectedIndexChanged += new System.EventHandler(this.tabControl1_SelectedIndexChanged);
            // 
            // authPage
            // 
            this.authPage.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(224)))), ((int)(((byte)(255)))));
            this.authPage.Controls.Add(this.pictureBox1);
            this.authPage.Controls.Add(this.readIDButton);
            this.authPage.Controls.Add(this.treeView1);
            this.authPage.Location = new System.Drawing.Point(0, 0);
            this.authPage.Name = "authPage";
            this.authPage.Size = new System.Drawing.Size(240, 217);
            this.authPage.Text = "Auth";
            // 
            // pictureBox1
            // 
            this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
            this.pictureBox1.Location = new System.Drawing.Point(79, 125);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(154, 90);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
            // 
            // treeView1
            // 
            this.treeView1.Location = new System.Drawing.Point(3, 3);
            this.treeView1.Name = "treeView1";
            this.treeView1.PathSeparator = "\\";
            this.treeView1.Size = new System.Drawing.Size(234, 118);
            this.treeView1.TabIndex = 2;
            // 
            // winePage
            // 
            this.winePage.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(224)))), ((int)(((byte)(255)))));
            this.winePage.Controls.Add(this.wineButton);
            this.winePage.Controls.Add(this.wineCountryLabel);
            this.winePage.Controls.Add(this.wineTypeLabel);
            this.winePage.Controls.Add(this.wineVineyardLabel);
            this.winePage.Controls.Add(this.wineYearLabel);
            this.winePage.Controls.Add(this.wineAuthPictureBox);
            this.winePage.Controls.Add(this.winePictureBox);
            this.winePage.Controls.Add(this.wineReviewTextBox);
            this.winePage.Location = new System.Drawing.Point(0, 0);
            this.winePage.Name = "winePage";
            this.winePage.Size = new System.Drawing.Size(240, 217);
            this.winePage.Text = "Wine";
            // 
            // wineButton
            // 
            this.wineButton.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.wineButton.Location = new System.Drawing.Point(0, 194);
            this.wineButton.Name = "wineButton";
            this.wineButton.Size = new System.Drawing.Size(40, 20);
            this.wineButton.TabIndex = 11;
            this.wineButton.Text = "Read";
            this.wineButton.Click += new System.EventHandler(this.readerClick);
            // 
            // wineCountryLabel
            // 
            this.wineCountryLabel.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.wineCountryLabel.Location = new System.Drawing.Point(140, 189);
            this.wineCountryLabel.Name = "wineCountryLabel";
            this.wineCountryLabel.Size = new System.Drawing.Size(100, 25);
            // 
            // wineTypeLabel
            // 
            this.wineTypeLabel.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.wineTypeLabel.Location = new System.Drawing.Point(41, 189);
            this.wineTypeLabel.Name = "wineTypeLabel";
            this.wineTypeLabel.Size = new System.Drawing.Size(100, 25);
            // 
            // wineVineyardLabel
            // 
            this.wineVineyardLabel.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.wineVineyardLabel.Location = new System.Drawing.Point(41, 163);
            this.wineVineyardLabel.Name = "wineVineyardLabel";
            this.wineVineyardLabel.Size = new System.Drawing.Size(100, 25);
            // 
            // wineYearLabel
            // 
            this.wineYearLabel.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.wineYearLabel.Location = new System.Drawing.Point(140, 163);
            this.wineYearLabel.Name = "wineYearLabel";
            this.wineYearLabel.Size = new System.Drawing.Size(100, 25);
            // 
            // wineAuthPictureBox
            // 
            this.wineAuthPictureBox.Location = new System.Drawing.Point(13, 166);
            this.wineAuthPictureBox.Name = "wineAuthPictureBox";
            this.wineAuthPictureBox.Size = new System.Drawing.Size(22, 22);
            this.wineAuthPictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
            // 
            // winePictureBox
            // 
            this.winePictureBox.Location = new System.Drawing.Point(0, 0);
            this.winePictureBox.Name = "winePictureBox";
            this.winePictureBox.Size = new System.Drawing.Size(240, 160);
            this.winePictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
            // 
            // wineReviewTextBox
            // 
            this.wineReviewTextBox.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(224)))), ((int)(((byte)(255)))));
            this.wineReviewTextBox.Location = new System.Drawing.Point(0, 0);
            this.wineReviewTextBox.Multiline = true;
            this.wineReviewTextBox.Name = "wineReviewTextBox";
            this.wineReviewTextBox.Size = new System.Drawing.Size(240, 217);
            this.wineReviewTextBox.TabIndex = 10;
            this.wineReviewTextBox.Visible = false;
            this.wineReviewTextBox.TextChanged += new System.EventHandler(this.wineReviewTextBox_TextChanged);
            // 
            // wineClient
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(240, 240);
            this.Controls.Add(this.tabControl1);
            this.KeyPreview = true;
            this.Location = new System.Drawing.Point(0, 0);
            this.MinimizeBox = false;
            this.Name = "wineClient";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.mainForm_KeyDown);
            this.tabControl1.ResumeLayout(false);
            this.authPage.ResumeLayout(false);
            this.winePage.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button readIDButton;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage authPage;
        private System.Windows.Forms.TreeView treeView1;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.TabPage winePage;
        private System.Windows.Forms.PictureBox winePictureBox;
        private System.Windows.Forms.Label wineVineyardLabel;
        private System.Windows.Forms.Label wineYearLabel;
        private System.Windows.Forms.Label wineCountryLabel;
        private System.Windows.Forms.Label wineTypeLabel;
        private System.Windows.Forms.TextBox wineReviewTextBox;
        private System.Windows.Forms.Button wineButton;
        private System.Windows.Forms.PictureBox wineAuthPictureBox;
        
    }
}

