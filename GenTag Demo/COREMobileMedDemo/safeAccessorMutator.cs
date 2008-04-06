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
using System.Collections;
using System.Net.Sockets;
using System.Threading;
using Microsoft.WindowsCE.Forms;

namespace COREMobileMedDemo
{
    public partial class mobileMedDemo : Form
    {
        #region Safe Accessors and Mutators

        private delegate void showMessageDelegate(string errorMessage);

        private void showMessage(string errorMessage)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new showMessageDelegate(showMessage), new object[] { errorMessage });
            }
            else
            {
                MessageBox.Show(errorMessage);
            }
        }

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
            if (pB.InvokeRequired)
            {
                pB.BeginInvoke(new setPhotoDelegateB(setPhoto), new object[] { pB, bA });

            }
            else
            {
                try
                {
                    if (pB.Image != null)
                        pB.Image.Dispose();
                    pB.Image = bA;
                    pB.Refresh();
                }
                catch (ArgumentException)
                { }
            }
        }

        private delegate void notifyDelegate(string caption, string text, bool critical);

        private void notify(string caption, string text, bool critical)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new notifyDelegate(notify), new object[] { caption, text, critical });
            }
            else
            {
                try
                {
                    if (popupNotification != null)
                        popupNotification.Dispose();
                }
                catch (Exception)
                { }
                popupNotification = new Notification();
                popupNotification.Caption = caption;
                popupNotification.Text = text;
                popupNotification.Critical = critical;
                popupNotification.InitialDuration = critical ? 60 : 5;

                popupNotification.Visible = true;
            }
        }

        private delegate void setWaitCursorDelegate(bool set);

        private void setWaitCursor(bool set)
        {
            if (this.InvokeRequired)
            {
                this.BeginInvoke(new setWaitCursorDelegate(setWaitCursor), new object[] { set });
            }
            else
            {
                if (set == true)
                {
                    Cursor.Current = Cursors.WaitCursor;
                }
                else
                {
                    Cursor.Current = Cursors.Default;
                }
                this.Refresh();
            }
        }

        private delegate void setLabelDelegate(Label tB, string desc);

        private void setLabel(Label tB, string val)
        {
            if (tB.InvokeRequired)
            {
                tB.Invoke(new setLabelDelegate(setLabel), new object[] { tB, val });
            }
            else
            {
                tB.Text = val;
            }
        }

        private delegate DialogResult showDialogDelegate(Form dialogBox);

        private DialogResult showDialog(Form dialogBox)
        {
            if (dialogBox.InvokeRequired)
            {
                return (DialogResult)dialogBox.Invoke(new showDialogDelegate(showDialog), new object[] { dialogBox });
            }
            else
            {
                return dialogBox.ShowDialog();
            }
        }

        private delegate decimal getNumericUpDownValueDelegate(NumericUpDown updown);

        private decimal getNumericUpDownValue(NumericUpDown updown)
        {
            if (updown.InvokeRequired)
            {
                return (decimal)updown.Invoke(new getNumericUpDownValueDelegate(getNumericUpDownValue), new object[] { updown });
            }
            else
            {
                return updown.Value;
            }
        }

        private delegate void setPhotoDelegate(PictureBox pB, byte[] bA);

        private void setPhoto(PictureBox pB, byte[] bA)
        {
            if (pB.InvokeRequired)
            {
                pB.BeginInvoke(new setPhotoDelegate(setPhoto), new object[] { pB, bA });
                return;
            }
            try
            {
                if (pB.Image != null)
                    pB.Image.Dispose();
                pB.Image = new Bitmap(new MemoryStream(bA));
                pB.Refresh();
            }
            catch (ArgumentException)
            {

            }
        }

        private delegate void setTextBoxDelegate(TextBox tb, string value);

        private void setTextBox(TextBox tb, string value)
        {
            if (tb.InvokeRequired)
            {
                tb.BeginInvoke(new setTextBoxDelegate(setTextBox), new object[] { tb, value });
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

        private delegate void setButtonTextDelegate(Button b, string c);

        private void setButtonText(Button b, string c)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setButtonTextDelegate(setButtonText), new object[] { b, c });
            }
            else
            {
                b.Text = c;
            }
        }

        private delegate void setButtonEnabledDelegate(Button b, bool enabled);

        private void setButtonEnabled(Button b, bool enabled)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setButtonEnabledDelegate(setButtonEnabled), new object[] { b, enabled });
            }
            else
            {
                b.Enabled = enabled;
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

        private delegate Color getTabPageBackgroundDelegate(TabPage p);

        private Color getTabPageBackground(TabPage p)
        {
            if (this.InvokeRequired)
            {
                return (Color)this.Invoke(new getTabPageBackgroundDelegate(getTabPageBackground), new object[] { p });
            }
            else
            {
                return p.BackColor;
            }
        }

        private delegate void setTabPageBackgroundDelegate(TabPage p, Color c);

        private void setTabPageBackground(TabPage p, Color c)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setTabPageBackgroundDelegate(setTabPageBackground), new object[] { p, c });
            }
            else
            {
                p.BackColor = c;
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