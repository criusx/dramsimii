using System;
using System.Drawing;
using System.Collections;
using System.Windows.Forms;
using OpenNETCF.Windows.Forms;

namespace OwnerDrawnListFWProject
{
    /// <summary>
    /// Summary description for CustomList.
    /// </summary>
    public class CustomMenu : OwnerDrawnList
    {
        const int DRAW_OFFSET = 4;
        public int action = 0;

        public CustomMenu()
        {
            //Set the item's height
            Graphics g = this.CreateGraphics();
            this.ItemHeight = Math.Max((int)(g.MeasureString("A", this.Font).Height * 1.3), this.ItemHeight);
            //Hide the scrollbar
            this.ShowScrollbar = false;
            //Hide the control for now
            this.Visible = false;
            g.Dispose();
        }

        //Show the menu
        public void Show(Point pt)
        {
            //Set the location
            this.Location = pt;
            //Select for the first item
            this.SelectedIndex = 0;
            this.Visible = true;
            this.BringToFront();
            this.Capture = true;
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            //Make a tracking behavior
            if (this.ClientRectangle.Contains(e.X, e.Y))
            {
                this.SelectedIndex = e.Y / this.ItemHeight;
                if (this.SelectedIndex == 0)
                    if (this.Items[0].ToString() == "Add")
                        this.action = 1;
                    else
                        this.action = -1;
                else
                    this.action = 0;
                this.Invalidate();
            }

        }

        //Implement a Click event
        protected override void OnClick(EventArgs e)
        {
            this.Hide();
            base.OnClick(e);
        }

        protected override void OnMouseUp(MouseEventArgs e)
        {
            this.SelectedIndex = -1;
            this.Capture = false;
            base.OnMouseUp(e);
        }

        public void onMouseUp(MouseEventArgs e)
        {
            this.OnMouseUp(e);
        }

        protected override void OnDrawItem(object sender, DrawItemEventArgs e)
        {
            if (e.Index < 0)
                return;

            Rectangle rc = e.Bounds;

            rc.X += DRAW_OFFSET;

            //Check if the item's selected
            //if (e.State == DrawItemState.Selected)
            //{
            //    e.DrawBackground(Color.DarkKhaki);
            //    rc.X += 1;
            //    rc.Y += 1;
            //}

            Brush textBrush = new SolidBrush(SystemColors.ControlText);

            //Draw item's text
            e.Graphics.DrawString(this.Items[e.Index].ToString(), e.Font, textBrush, rc);
            //Draw a 3D line underneath 
            e.Graphics.DrawLine(new Pen(Color.White), 0, e.Bounds.Bottom - 1, e.Bounds.Width, e.Bounds.Bottom - 1);
            e.Graphics.DrawLine(new Pen(Color.Gray), 0, e.Bounds.Bottom, e.Bounds.Width, e.Bounds.Bottom);
            //Call the base's OnDrawEvent	
            base.OnDrawItem(sender, e);
        }

        public void Add(object value)
        {
            this.Insert(Items.Count, value);
            ////Adjust the height
            //this.Height = ((int)this.Height / ItemHeight) * ItemHeight;

            ////How many items are visible
            //int viewableItemCount = this.ClientSize.Height / this.ItemHeight;

            //this.vScroll.Bounds = new Rectangle(this.ClientSize.Width - SCROLL_WIDTH,
            //    0,
            //    SCROLL_WIDTH,
            //    this.ClientSize.Height);


            //// Determine if scrollbars are needed
            //if (this.listItems.Count > viewableItemCount)
            //{
            //    this.vScroll.Visible = true;
            //    this.vScroll.LargeChange = viewableItemCount;
            //    this.m_bmpOffscreen = new Bitmap(this.ClientSize.Width - SCROLL_WIDTH, this.ClientSize.Height);
            //}
            //else
            //{
            //    //showScrollbar = false;
            //    this.vScroll.Visible = false;
            //    this.vScroll.LargeChange = this.listItems.Count;
            //    this.m_bmpOffscreen = new Bitmap(this.ClientSize.Width, this.ClientSize.Height);
            //}

            //this.vScroll.Maximum = this.listItems.Count - 1;
        }


    }
}
