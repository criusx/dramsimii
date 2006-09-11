/// Taken from http://www.opennetcf.org/PermaLink.aspx?guid=d88b1a1a-a866-4247-a0c8-94ab3c124f63
/// �2003 Alex Yakhnin. OpenNETCF Advisory Board
using System;
using System.Drawing;
using System.Windows.Forms;
using System.Drawing.Imaging;
using OpenNETCF.Windows.Forms;
using ListItemNS;

namespace OwnerDrawnListFWProject
{
    /// <summary>
    /// Summary description for CustomListBox.
    /// </summary>
    public class CustomListBox : OwnerDrawnList
    {
        const int DRAW_OFFSET = 4;

        private ImageList imageList = null;
        private bool wrapText = false;
        ImageAttributes imageAttr = new ImageAttributes();

        public bool WrapText
        {
            get
            {
                return wrapText;
            }
            set
            {
                wrapText = value;
            }
        }

        public ImageList ImageList
        {
            get
            {
                return imageList;
            }
            set
            {
                imageList = value;
            }
        }

        public CustomListBox()
        {
            this.ShowScrollbar = true;
            this.ForeColor = Color.Black;

            //Set the item's height
            Graphics g = this.CreateGraphics();

            if (wrapText)
                this.ItemHeight = 2 * Math.Max((int)(g.MeasureString("A", this.Font).Height), this.ItemHeight) + 2;
            else
                this.ItemHeight = Math.Max((int)(g.MeasureString("A", this.Font).Height), this.ItemHeight) + 4;

            g.Dispose();
        }

        protected override void OnDrawItem(object sender, DrawItemEventArgs e)
        {
            Brush textBrush; //Brush for the text

            Rectangle rc = e.Bounds;
            rc.X += DRAW_OFFSET;

            //Get the ListItem
            //ListItem item;

            if (e.Index >= this.Items.Count)
                return;

            if (e.State == DrawItemState.Selected)
            {
                //Highlighted
                e.DrawBackground();
                textBrush = new SolidBrush(SystemColors.HighlightText);
            }
            else
            {
                if (((ListItem)this.Items[e.Index]).missingAdded == -2)
                    e.DrawBackground(Color.DarkRed);
                if (((ListItem)this.Items[e.Index]).missingAdded == -1)
                    e.DrawBackground(Color.Tomato);
                else if (((ListItem)this.Items[e.Index]).missingAdded == 0)
                    e.DrawBackground(this.BackColor);
                else
                    e.DrawBackground(Color.LightGreen);
            }

            textBrush = new SolidBrush(this.ForeColor);


            //Check if the item has a image
            //if (((ListItem)this.Items[e.Index]).ImageIndex > -1)
            //{
            //    Image img = imageList.Images[((ListItem)this.Items[e.Index]).ImageIndex];
            //    if (img != null)
            //    {
            //        imageAttr = new ImageAttributes();

            //        //Set the transparency key
            //        imageAttr.SetColorKey(BackgroundImageColor(img), BackgroundImageColor(img));

            //        //Image's rectangle
            //        Rectangle imgRect = new Rectangle(2, rc.Y + 1, img.Width, img.Height);
            //        //Draw the image
            //        e.Graphics.DrawImage(img, imgRect, 0, 0, img.Width, img.Height, GraphicsUnit.Pixel, imageAttr);
            //        //Shift the text to the right
            //        rc.X += img.Width + 2;
            //    }
            //}

            //Draw item's text
            if (((ListItem)this.Items[e.Index]).shortDesc != "")
                e.Graphics.DrawString(((ListItem)this.Items[e.Index]).shortDesc, e.Font, textBrush, rc);
            else
                e.Graphics.DrawString(((ListItem)this.Items[e.Index]).RFIDNum, e.Font, textBrush, rc);

            //Draw the line
            e.Graphics.DrawLine(new Pen(Color.Navy), 0, e.Bounds.Bottom, e.Bounds.Width, e.Bounds.Bottom);
            //Call the base's OnDrawEvent	
            base.OnDrawItem(sender, e);
        }

        private Color BackgroundImageColor(Image image)
        {
            Bitmap bmp = new Bitmap(image);
            return bmp.GetPixel(0, 0);
        }
    }
}
