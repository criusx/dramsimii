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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(assayResultsChooser));
            this.assayResultsListView = new System.Windows.Forms.ListView();
            this.selectButton = new System.Windows.Forms.Button();
            this.resetButton = new System.Windows.Forms.Button();
            this.afterMessageTextBox = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // assayResultsListView
            // 
            resources.ApplyResources(this.assayResultsListView, "assayResultsListView");
            this.assayResultsListView.BackColor = System.Drawing.SystemColors.Control;
            this.assayResultsListView.Name = "assayResultsListView";
            this.assayResultsListView.SelectedIndexChanged += new System.EventHandler(this.assayResultsListView_SelectedIndexChanged);
            // 
            // selectButton
            // 
            resources.ApplyResources(this.selectButton, "selectButton");
            this.selectButton.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.selectButton.Name = "selectButton";
            // 
            // resetButton
            // 
            resources.ApplyResources(this.resetButton, "resetButton");
            this.resetButton.DialogResult = System.Windows.Forms.DialogResult.Abort;
            this.resetButton.Name = "resetButton";
            // 
            // afterMessageTextBox
            // 
            resources.ApplyResources(this.afterMessageTextBox, "afterMessageTextBox");
            this.afterMessageTextBox.Name = "afterMessageTextBox";
            this.afterMessageTextBox.ReadOnly = true;
            // 
            // assayResultsChooser
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            resources.ApplyResources(this, "$this");
            this.Controls.Add(this.afterMessageTextBox);
            this.Controls.Add(this.resetButton);
            this.Controls.Add(this.selectButton);
            this.Controls.Add(this.assayResultsListView);
            this.Name = "assayResultsChooser";
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