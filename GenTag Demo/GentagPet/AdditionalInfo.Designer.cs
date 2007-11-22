namespace GentagPet
{
    partial class AdditionalInfo
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
            System.Windows.Forms.ListViewItem listViewItem1 = new System.Windows.Forms.ListViewItem();
            System.Windows.Forms.ListViewItem listViewItem2 = new System.Windows.Forms.ListViewItem();
            System.Windows.Forms.ListViewItem listViewItem3 = new System.Windows.Forms.ListViewItem();
            System.Windows.Forms.ListViewItem listViewItem4 = new System.Windows.Forms.ListViewItem();
            System.Windows.Forms.ListViewItem listViewItem5 = new System.Windows.Forms.ListViewItem();
            System.Windows.Forms.ListViewItem listViewItem6 = new System.Windows.Forms.ListViewItem();
            System.Windows.Forms.ListViewItem listViewItem7 = new System.Windows.Forms.ListViewItem();
            System.Windows.Forms.ListViewItem listViewItem8 = new System.Windows.Forms.ListViewItem();
            System.Windows.Forms.ListViewItem listViewItem9 = new System.Windows.Forms.ListViewItem();
            System.Windows.Forms.ListViewItem listViewItem10 = new System.Windows.Forms.ListViewItem();
            System.Windows.Forms.ListViewItem listViewItem11 = new System.Windows.Forms.ListViewItem();
            System.Windows.Forms.ListViewItem listViewItem12 = new System.Windows.Forms.ListViewItem();
            System.Windows.Forms.ListViewItem listViewItem13 = new System.Windows.Forms.ListViewItem();
            System.Windows.Forms.ListViewItem listViewItem14 = new System.Windows.Forms.ListViewItem();
            System.Windows.Forms.ListViewItem listViewItem15 = new System.Windows.Forms.ListViewItem();
            System.Windows.Forms.ListViewItem listViewItem16 = new System.Windows.Forms.ListViewItem();
            System.Windows.Forms.ListViewItem listViewItem17 = new System.Windows.Forms.ListViewItem();
            this.dataListView = new System.Windows.Forms.ListView();
            this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
            this.SuspendLayout();
            // 
            // dataListView
            // 
            this.dataListView.Columns.Add(this.columnHeader1);
            this.dataListView.Columns.Add(this.columnHeader2);
            listViewItem1.Text = "ID";
            listViewItem1.SubItems.Add("123");
            listViewItem2.Text = "Owner";
            listViewItem3.Text = "Contact";
            listViewItem4.Text = "Breed";
            listViewItem5.Text = "Tag code";
            listViewItem6.Text = "Name";
            listViewItem7.Text = "Home Ph.";
            listViewItem8.Text = "Cell Ph.";
            listViewItem9.Text = "Work Ph.";
            listViewItem10.Text = "Email";
            listViewItem11.Text = "Diet";
            listViewItem12.Text = "Medical";
            listViewItem13.Text = "Medication";
            listViewItem14.Text = "Vet";
            listViewItem15.Text = "Vet Ph.";
            listViewItem16.Text = "Vet Addr.";
            listViewItem17.Text = "Reward";
            this.dataListView.Items.Add(listViewItem1);
            this.dataListView.Items.Add(listViewItem2);
            this.dataListView.Items.Add(listViewItem3);
            this.dataListView.Items.Add(listViewItem4);
            this.dataListView.Items.Add(listViewItem5);
            this.dataListView.Items.Add(listViewItem6);
            this.dataListView.Items.Add(listViewItem7);
            this.dataListView.Items.Add(listViewItem8);
            this.dataListView.Items.Add(listViewItem9);
            this.dataListView.Items.Add(listViewItem10);
            this.dataListView.Items.Add(listViewItem11);
            this.dataListView.Items.Add(listViewItem12);
            this.dataListView.Items.Add(listViewItem13);
            this.dataListView.Items.Add(listViewItem14);
            this.dataListView.Items.Add(listViewItem15);
            this.dataListView.Items.Add(listViewItem16);
            this.dataListView.Items.Add(listViewItem17);
            this.dataListView.Location = new System.Drawing.Point(4, 3);
            this.dataListView.Name = "dataListView";
            this.dataListView.Size = new System.Drawing.Size(233, 207);
            this.dataListView.TabIndex = 0;
            this.dataListView.View = System.Windows.Forms.View.Details;
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "Property";
            this.columnHeader1.Width = 67;
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "Value";
            this.columnHeader2.Width = 147;
            // 
            // AdditionalInfo
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(240, 214);
            this.Controls.Add(this.dataListView);
            this.Name = "AdditionalInfo";
            this.Text = "AdditionalInfo";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListView dataListView;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.ColumnHeader columnHeader2;


    }
}