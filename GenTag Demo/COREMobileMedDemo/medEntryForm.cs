using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace COREMobileMedDemo
{
    class medEntryForm : Form
    {
        private Label label1;
        private Label label2;
        private Label medNameLabel;
        private Label patientNameLabel;
        private NumericUpDown medDosageUpDown;
        private PictureBox medImagePB;
        private Label label5;
        private Label label6;
        private Label label7;
        private Button noButton;
        private Button okButton;
        private PictureBox medPB;

        public medEntryForm()
        {
            InitializeComponent();
        }

        public PictureBox medicinePicture
        {
            get
            {
                return medImagePB;
            }
        }

        public Label patientName
        {
            get
            {
                return patientNameLabel;
            }
        }

        public Label medicationName
        {
            get
            {
                return medNameLabel;
            }
        }

        public decimal dosageValue
        {
            get
            {
                return medDosageUpDown.Value;
            }
        }

        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.medNameLabel = new System.Windows.Forms.Label();
            this.patientNameLabel = new System.Windows.Forms.Label();
            this.medDosageUpDown = new System.Windows.Forms.NumericUpDown();
            this.medImagePB = new System.Windows.Forms.PictureBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.noButton = new System.Windows.Forms.Button();
            this.okButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(3, 2);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(82, 20);
            this.label1.Text = "Patient Name";
            this.label1.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(3, 22);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(82, 20);
            this.label2.Text = "Medication";
            this.label2.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // medNameLabel
            // 
            this.medNameLabel.Location = new System.Drawing.Point(91, 22);
            this.medNameLabel.Name = "medNameLabel";
            this.medNameLabel.Size = new System.Drawing.Size(146, 20);
            this.medNameLabel.Text = "Medication";
            // 
            // patientNameLabel
            // 
            this.patientNameLabel.Location = new System.Drawing.Point(91, 2);
            this.patientNameLabel.Name = "patientNameLabel";
            this.patientNameLabel.Size = new System.Drawing.Size(146, 20);
            this.patientNameLabel.Text = "Medication";
            // 
            // medDosageUpDown
            // 
            this.medDosageUpDown.Increment = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.medDosageUpDown.Location = new System.Drawing.Point(109, 156);
            this.medDosageUpDown.Maximum = new decimal(new int[] {
            5000,
            0,
            0,
            0});
            this.medDosageUpDown.Minimum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.medDosageUpDown.Name = "medDosageUpDown";
            this.medDosageUpDown.Size = new System.Drawing.Size(64, 22);
            this.medDosageUpDown.TabIndex = 4;
            this.medDosageUpDown.Value = new decimal(new int[] {
            500,
            0,
            0,
            0});
            // 
            // medImagePB
            // 
            this.medImagePB.Location = new System.Drawing.Point(91, 46);
            this.medImagePB.Name = "medImagePB";
            this.medImagePB.Size = new System.Drawing.Size(100, 92);
            // 
            // label5
            // 
            this.label5.Location = new System.Drawing.Point(179, 158);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(95, 20);
            this.label5.Text = "mg";
            // 
            // label6
            // 
            this.label6.Location = new System.Drawing.Point(8, 158);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(95, 20);
            this.label6.Text = "Dose";
            this.label6.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label7
            // 
            this.label7.Location = new System.Drawing.Point(3, 73);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(82, 65);
            this.label7.Text = "Medication Image";
            this.label7.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // noButton
            // 
            this.noButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.noButton.Location = new System.Drawing.Point(142, 192);
            this.noButton.Name = "noButton";
            this.noButton.Size = new System.Drawing.Size(95, 45);
            this.noButton.TabIndex = 8;
            this.noButton.Text = "Cancel";
            // 
            // okButton
            // 
            this.okButton.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.okButton.Location = new System.Drawing.Point(3, 192);
            this.okButton.Name = "okButton";
            this.okButton.Size = new System.Drawing.Size(95, 45);
            this.okButton.TabIndex = 8;
            this.okButton.Text = "Accept";
            // 
            // medEntryForm
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Inherit;
            this.ClientSize = new System.Drawing.Size(240, 240);
            this.Controls.Add(this.noButton);
            this.Controls.Add(this.okButton);
            this.Controls.Add(this.medImagePB);
            this.Controls.Add(this.medDosageUpDown);
            this.Controls.Add(this.patientNameLabel);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.medNameLabel);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Location = new System.Drawing.Point(0, 0);
            this.Name = "medEntryForm";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.ResumeLayout(false);

        }
    }
}
