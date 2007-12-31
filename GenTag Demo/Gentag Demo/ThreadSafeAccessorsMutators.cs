using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.IO;

namespace GentagDemo
{
    public partial class demoClient : Form
    {
        #region Safe Accessors and Mutators

        private delegate string getComboBoxDelegate(ComboBox cb);

        private string getComboBox(ComboBox cb)
        {
            if (this.InvokeRequired)
            {
                return (string)this.Invoke(new getComboBoxDelegate(getComboBox), new object[] { cb });
            }
            else
            {
                return cb.SelectedItem.ToString();
            }
        }

        private delegate void setPhotoDelegateB(PictureBox pB, Image bA);

        private void setPhoto(PictureBox pB, Image bA)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setPhotoDelegateB(setPhoto), new object[] { pB, bA });
                return;
            }
            try
            {
                if (pB.Image != null)
                    pB.Image.Dispose();
                pB.Image = bA;
                pB.Refresh();
            }
            catch (ArgumentException ex)
            {

            }
        }

        private delegate void setWaitCursorDelegate(bool set);

        private void setWaitCursor(bool set)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setWaitCursorDelegate(setWaitCursor), new object[] { set });
            }
            if (set == true)
                Cursor.Current = Cursors.WaitCursor;
            else
                Cursor.Current = Cursors.Default;
        }

        private delegate void setLabelDelegate(Label tB, string desc);

        private void setLabel(Label tB, string val)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setLabelDelegate(setLabel), new object[] { tB, val });
                return;
            }
            tB.Text = val;
        }

        private delegate void setPhotoDelegate(PictureBox pB, byte[] bA);

        private void setPhoto(PictureBox pB, byte[] bA)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setPhotoDelegate(setPhoto), new object[] { pB, bA });
                return;
            }
            try
            {
                if (pB.Image != null)
                    pB.Image.Dispose();
                pB.Image = new Bitmap(new MemoryStream(bA));
                pB.Refresh();
            }
            catch (ArgumentException ex)
            {

            }
        }

        private delegate void setTextBoxDelegate(TextBox tb, string value);

        private void setTextBox(TextBox tb, string value)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setTextBoxDelegate(setTextBox), new object[] { tb, value });
            }      
            else
                tb.Text = value;            
        }

        private delegate void setProgressBarDelegate(ProgressBar pb, int value);

        private void setProgressBar(ProgressBar pb, int value)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setProgressBarDelegate(setProgressBar), new object[] { pb, value });
            }
            else
            {
                if (pb.Maximum >= value)
                    pb.Value = value;
            }
        }

        private delegate void setProgressBarMaxDelegate(ProgressBar pb, int value);

        private void setProgressBarMax(ProgressBar pb, int value)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setProgressBarMaxDelegate(setProgressBarMax), new object[] { pb, value });
            }
            else
            {
                pb.Maximum = value;
            }
        }

        private delegate void setButtonColorDelegate(Button b, Color c);

        private void setButtonColor(Button b, Color c)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setButtonColorDelegate(setButtonColor), new object[] { b, c });
            }
            else
            {
                b.BackColor = c;
            }
        }

        private delegate Color getButtonColorDelegate(Button b);

        private Color getButtonColor(Button b)
        {
            if (this.InvokeRequired)
            {
                return (Color)this.Invoke(new getButtonColorDelegate(getButtonColor), new object[] { b });
            }
            else
            {
                return b.BackColor;
            }
        }

        private delegate string getTextBoxDelegate(TextBox tb);

        private string getTextBox(TextBox tb)
        {
            if (this.InvokeRequired)
            {
                return (string)this.Invoke(new getTextBoxDelegate(getTextBox), new object[] { tb });
            }
            else
            {
                return tb.Text;
            }
        }

        #endregion
    }
}
