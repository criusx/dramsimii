using System.Drawing;
using System.Windows.Forms;
using System.IO;
using System;

namespace GentagDemo
{
    public partial class assayResultsChooser : Form
    {
        public assayResultsChooser()
        {
            InitializeComponent();

            oldHeight = -1;
        }

        private delegate void setImagesDelegate(assayWS.assayImage[] imageList);

        public void setImages(assayWS.assayImage[] imageList)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setImagesDelegate(setImages), new object[] { imageList });
            }
            else
            {
                if (imageList != null)
                {
                    selectButton.Enabled = false;

                    if (imageList.Length < 1)
                        return;

                    assayResultsListView.BeginUpdate();

                    ImageList il = new ImageList();

                    for (int i = 0; i < imageList.Length; i++)
                    {
                        il.Images.Add(new Bitmap(new MemoryStream(imageList[i].image)));
                    }


                    il.ImageSize = new Size(new Bitmap(new MemoryStream(imageList[0].image)).Width, new Bitmap(new MemoryStream(imageList[0].image)).Height);

                    assayResultsListView.LargeImageList = il;

                    for (int i = 0; i < imageList.Length; i++)
                    {
                        ListViewItem lvi = new ListViewItem();

                        lvi.ImageIndex = i;

                        assayResultsListView.Items.Add(lvi);
                    }

                    assayResultsListView.EndUpdate();
                }
                else
                {
                    // TODO: define behavior better here
                }
            }
        }

        private delegate void setAfterMessageDelegate(string message);

        public void setAfterMessage(string message)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setAfterMessageDelegate(setAfterMessage), new object[] { message });
            }
            else
            {
                afterMessageTextBox.Text = message;
            }
        }

        public int SelectedIndex
        {

            get
            {
                ListView.SelectedIndexCollection index = assayResultsListView.SelectedIndices;
                return index[0];
            }
        }

        private void assayResultsListView_SelectedIndexChanged(object sender, System.EventArgs e)
        {
            try
            {
                if (this.SelectedIndex >= 0)
                    selectButton.Enabled = true;
            }
            catch (ArgumentOutOfRangeException)
            {
                selectButton.Enabled = false;
            }
        }

        private int oldHeight;

        protected override void OnResize(System.EventArgs e)
        {
            base.OnResize(e);
            if (oldHeight <= 0)
                oldHeight = this.Height;
            if (oldHeight > 0)
            {
                int newHeight = this.Height;
                assayResultsListView.Height = Convert.ToInt32(Convert.ToSingle(newHeight) / Convert.ToSingle(oldHeight) * Convert.ToSingle(assayResultsListView.Height));
            }
        }
    }
}