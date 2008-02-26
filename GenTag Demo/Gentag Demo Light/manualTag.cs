using System.Windows.Forms;
using System.Globalization;

namespace GentagDemo
{
    public partial class manualTag : Form
    {
        public manualTag()
        {
            InitializeComponent();
            this.WindowState = FormWindowState.Normal;
        }        

        public string Value
        {
            get { return textBox1.Text; }
        }

        private void textBox1_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (textBox1.Text.Length < 16 && "0123456789ABCDEF".IndexOf(char.ToUpper(e.KeyChar,CultureInfo.CurrentCulture)) >= 0)
            {
                textBox1.Text += char.ToUpper(e.KeyChar, CultureInfo.CurrentCulture);
                textBox1.SelectionStart = textBox1.Text.Length;
            }
            else if (e.KeyChar == (int)Keys.Back)
            {
                if (textBox1.Text.Length > 0)
                    textBox1.Text = textBox1.Text.Substring(0, textBox1.Text.Length - 1);
            }
            if (textBox1.Text.Length < 16)
                button1.Enabled = false;
            else
                button1.Enabled = true;
            e.Handled = true;
        }


        private void manualTag_Activated(object sender, System.EventArgs e)
        {
            textBox1.Text = "";
            button1.Enabled = false;
        }
    }
}