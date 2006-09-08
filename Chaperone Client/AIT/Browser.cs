using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Threading;


namespace WJ2
{
    public partial class Browser : Form
    {
        public Browser()
        {
            InitializeComponent();
            webBrowser1.DocumentCompleted += new WebBrowserDocumentCompletedEventHandler(doneLoading);
        }

        private void menuItem1_Click(object sender, EventArgs e)
        {
            this.Hide();
        }
        
        public void setUrlAndShow(Uri newUrl)
        {
            webBrowser1.Url = newUrl;
            //c = Cursors.WaitCursor;
            //c.Show();
        }

        private void doneLoading(Object sender, WebBrowserDocumentCompletedEventArgs e)
        {
            //.Cursor.Hide();
            this.Show();
        }
    }
}