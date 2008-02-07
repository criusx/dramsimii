namespace GentagDemo
{
    partial class assayResultsChooser
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
            this.assayResultsListView = new System.Windows.Forms.ListView();
            this.selectButton = new System.Windows.Forms.Button();
            this.resetButton = new System.Windows.Forms.Button();
            this.afterMessageTextBox = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // assayResultsListView
            // 
            this.assayResultsListView.BackColor = System.Drawing.SystemColors.Control;
            this.assayResultsListView.Location = new System.Drawing.Point(4, 4);
            this.assayResultsListView.Name = "assayResultsListView";
            this.assayResultsListView.Size = new System.Drawing.Size(233, 178);
            this.assayResultsListView.TabIndex = 0;
            this.assayResultsListView.SelectedIndexChanged += new System.EventHandler(this.assayResultsListView_SelectedIndexChanged);
            // 
            // selectButton
            // 
            this.selectButton.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.selectButton.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.selectButton.Enabled = false;
            this.selectButton.Location = new System.Drawing.Point(3, 225);
            this.selectButton.Name = "selectButton";
            this.selectButton.Size = new System.Drawing.Size(60, 15);
            this.selectButton.TabIndex = 1;
            this.selectButton.Text = "Select";
            // 
            // resetButton
            // 
            this.resetButton.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.resetButton.DialogResult = System.Windows.Forms.DialogResult.Abort;
            this.resetButton.Location = new System.Drawing.Point(177, 225);
            this.resetButton.Name = "resetButton";
            this.resetButton.Size = new System.Drawing.Size(60, 15);
            this.resetButton.TabIndex = 1;
            this.resetButton.Text = "Reset";
            // 
            // afterMessageTextBox
            // 
            this.afterMessageTextBox.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.afterMessageTextBox.Location = new System.Drawing.Point(4, 184);
            this.afterMessageTextBox.Multiline = true;
            this.afterMessageTextBox.Name = "afterMessageTextBox";
            this.afterMessageTextBox.ReadOnly = true;
            this.afterMessageTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.afterMessageTextBox.Size = new System.Drawing.Size(233, 36);
            this.afterMessageTextBox.TabIndex = 2;
            // 
            // assayResultsChooser
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(240, 240);
            this.Controls.Add(this.afterMessageTextBox);
            this.Controls.Add(this.resetButton);
            this.Controls.Add(this.selectButton);
            this.Controls.Add(this.assayResultsListView);
            this.Location = new System.Drawing.Point(0, 0);
            this.Name = "assayResultsChooser";
            this.Text = "assayResultsChooser";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListView assayResultsListView;
        private System.Windows.Forms.Button selectButton;
        private System.Windows.Forms.Button resetButton;
        private System.Windows.Forms.TextBox afterMessageTextBox;
    }
}