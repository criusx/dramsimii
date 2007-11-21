namespace GentagPet
{
    partial class gentagPet
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;
        private System.Windows.Forms.MainMenu mainMenu1;

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
            this.mainMenu1 = new System.Windows.Forms.MainMenu();
            this.petContactInfoLabel = new System.Windows.Forms.Label();
            this.petBreedLabel = new System.Windows.Forms.Label();
            this.petOwnerLabel = new System.Windows.Forms.Label();
            this.petPhonenumberLabel = new System.Windows.Forms.Label();
            this.petPB = new System.Windows.Forms.PictureBox();
            this.menuItem1 = new System.Windows.Forms.MenuItem();
            this.menuItem2 = new System.Windows.Forms.MenuItem();
            this.menuItem3 = new System.Windows.Forms.MenuItem();
            this.menuItem4 = new System.Windows.Forms.MenuItem();
            this.menuItem5 = new System.Windows.Forms.MenuItem();
            this.SuspendLayout();
            // 
            // mainMenu1
            // 
            this.mainMenu1.MenuItems.Add(this.menuItem1);
            this.mainMenu1.MenuItems.Add(this.menuItem4);
            // 
            // petContactInfoLabel
            // 
            this.petContactInfoLabel.Location = new System.Drawing.Point(109, 180);
            this.petContactInfoLabel.Name = "petContactInfoLabel";
            this.petContactInfoLabel.Size = new System.Drawing.Size(131, 34);
            this.petContactInfoLabel.Text = "Contact Info";
            // 
            // petBreedLabel
            // 
            this.petBreedLabel.Location = new System.Drawing.Point(3, 184);
            this.petBreedLabel.Name = "petBreedLabel";
            this.petBreedLabel.Size = new System.Drawing.Size(100, 25);
            this.petBreedLabel.Text = "Breed";
            // 
            // petOwnerLabel
            // 
            this.petOwnerLabel.Location = new System.Drawing.Point(3, 159);
            this.petOwnerLabel.Name = "petOwnerLabel";
            this.petOwnerLabel.Size = new System.Drawing.Size(100, 25);
            this.petOwnerLabel.Text = "Owner";
            // 
            // petPhonenumberLabel
            // 
            this.petPhonenumberLabel.Location = new System.Drawing.Point(109, 163);
            this.petPhonenumberLabel.Name = "petPhonenumberLabel";
            this.petPhonenumberLabel.Size = new System.Drawing.Size(128, 20);
            this.petPhonenumberLabel.Text = "Phone";
            // 
            // petPB
            // 
            this.petPB.Location = new System.Drawing.Point(0, 0);
            this.petPB.Name = "petPB";
            this.petPB.Size = new System.Drawing.Size(240, 160);
            this.petPB.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
            // 
            // menuItem1
            // 
            this.menuItem1.MenuItems.Add(this.menuItem2);
            this.menuItem1.MenuItems.Add(this.menuItem3);
            this.menuItem1.Text = "Options";
            // 
            // menuItem2
            // 
            this.menuItem2.Text = "Lookup";
            this.menuItem2.Click += new System.EventHandler(this.menuItem2_Click);
            // 
            // menuItem3
            // 
            this.menuItem3.Enabled = false;
            this.menuItem3.Text = "Additional Info";
            // 
            // menuItem4
            // 
            this.menuItem4.MenuItems.Add(this.menuItem5);
            this.menuItem4.Text = "Program";
            // 
            // menuItem5
            // 
            this.menuItem5.Text = "Quit";
            // 
            // gentagPet
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(240, 214);
            this.Controls.Add(this.petContactInfoLabel);
            this.Controls.Add(this.petBreedLabel);
            this.Controls.Add(this.petOwnerLabel);
            this.Controls.Add(this.petPhonenumberLabel);
            this.Controls.Add(this.petPB);
            this.Location = new System.Drawing.Point(0, 0);
            this.Menu = this.mainMenu1;
            this.Name = "gentagPet";
            this.Text = "Gentag Pet";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label petContactInfoLabel;
        private System.Windows.Forms.Label petBreedLabel;
        private System.Windows.Forms.Label petOwnerLabel;
        private System.Windows.Forms.Label petPhonenumberLabel;
        private System.Windows.Forms.PictureBox petPB;
        private System.Windows.Forms.MenuItem menuItem1;
        private System.Windows.Forms.MenuItem menuItem2;
        private System.Windows.Forms.MenuItem menuItem3;
        private System.Windows.Forms.MenuItem menuItem4;
        private System.Windows.Forms.MenuItem menuItem5;
    }
}

