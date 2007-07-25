using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Microsoft.WindowsMobile.Forms;
using System.IO;
using System.Net;
using SiritReader;
using System.Collections;
using System.Net.Sockets;
using System.Threading;

namespace COREMobileMedDemo
{
    public partial class mobileMedDemo : Form
    {
        #region Safe Accessors and Mutators

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

        private delegate Color getPanelDelegate(Panel p);

        private Color getPanel(Panel p)
        {
            if (this.InvokeRequired)
            {
                return (Color)this.Invoke(new getPanelDelegate(getPanel), new object[] { p });
            }
            return p.BackColor;
        }

        private delegate Color getButtonColorDelegate(Button p);

        private Color getButtonColor(Button p)
        {
            if (this.InvokeRequired)
            {
                return (Color)this.Invoke(new getButtonColorDelegate(getButtonColor), new object[] { p });
            }
            return p.BackColor;
        }

        private delegate void setPanelDelegate(Panel p, Color c);

        private void setPanel(Panel p, Color c)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setPanelDelegate(setPanel), new object[] { p, c });
                return;
            }
            p.BackColor = c;
        }

        private delegate void setButtonColorDelegate(Button p, Color c);

        private void setButtonColor(Button p, Color c)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setButtonColorDelegate(setButtonColor), new object[] { p, c });
                return;
            }
            p.BackColor = c;
        }

        private delegate void setCheckBoxDelegate(CheckBox cB, bool check);

        private void setCheckBox(CheckBox cB, bool check)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setCheckBoxDelegate(setCheckBox), new object[] { cB, check });
                return;
            }
            cB.Checked = check;
        }

        private delegate bool getCheckBoxDelegate(CheckBox cB);

        private bool getCheckBox(CheckBox cB)
        {
            if (this.InvokeRequired)
            {
                return (bool)this.Invoke(new getCheckBoxDelegate(getCheckBox), new object[] { cB });
            }
            return cB.Checked;
        }

        private delegate int getComboBoxIndexDelegate(ComboBox cB);

        private int getComboBoxIndex(ComboBox cB)
        {
            if (this.InvokeRequired)
            {
                return (int)this.Invoke(new getComboBoxIndexDelegate(getComboBoxIndex), new object[] { cB });
            }
            return cB.SelectedIndex;
        }

        private delegate void setComboBoxIndexDelegate(ComboBox cB, int index);

        private void setComboBoxIndex(ComboBox cB, int index)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setComboBoxIndexDelegate(setComboBoxIndex), new object[] { cB, index });
                return;
            }
            try
            {
                cB.SelectedIndex = index;
            }
            catch (ArgumentOutOfRangeException)
            {
                cB.SelectedIndex = 0;
            }
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

        private delegate void setTextBoxDelegate(TextBox tB, string desc);

        private void setTextBox(TextBox tB, string val)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setTextBoxDelegate(setTextBox), new object[] { tB, val });
                return;
            }
            tB.Text = val;
        }

        private delegate void setProgressBarDelegate(ProgressBar pB, int val);

        private void setProgressBar(ProgressBar pB, int val)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setProgressBarDelegate(setProgressBar), new object[] { pB, val });
                return;
            }
            if (val > pB.Maximum)
                pB.Value = pB.Maximum;
            else if (val < pB.Minimum)
                pB.Value = pB.Minimum;
            else
                pB.Value = val;
        }

        private delegate string getTextBoxDelegate(TextBox tB);

        private string getTextBox(TextBox tB)
        {
            if (this.InvokeRequired)
            {
                return (string)this.Invoke(new getTextBoxDelegate(getTextBox), new object[] { tB });
            }
            return tB.Text;
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
                pB.Image = bA;
                pB.Refresh();
            }
            catch (ArgumentException)
            {

            }
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
                pB.Image = new Bitmap(new MemoryStream(bA));
                pB.Refresh();
            }
            catch (ArgumentException)
            {

            }
        }
        #endregion
    }
}