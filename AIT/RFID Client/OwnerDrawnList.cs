/// Taken from http://www.opennetcf.org/PermaLink.aspx?guid=d88b1a1a-a866-4247-a0c8-94ab3c124f63
/// ©2003 Alex Yakhnin. OpenNETCF Advisory Board
using System;
using System.Drawing;
using System.Collections;
using System.Windows.Forms;


namespace OpenNETCF.Windows.Forms
{

    //Represents the method that will handle the DrawItem event
    public delegate void DrawItemEventHandler(object sender, DrawItemEventArgs e);


    #region DrawItemState enum
    //Specifies the state of an item that is being drawn.
    public enum DrawItemState
    {
        None = 0,
        Selected = 1,
        Disabled = 4,
        Focus = 16
    }
    #endregion

    #region DrawItemEventArgs

    /// <summary>
    /// Provides data for the DrawItem event.
    /// </summary>
    public class DrawItemEventArgs : System.EventArgs
    {
        //private members
        private Color backColor;
        private Color foreColor;
        private Font font;
        private int index;
        private Graphics graphics;
        private Rectangle rect;
        private DrawItemState state;
        

        /// <summary>
        /// Initializes a new instance
        /// </summary>
        public DrawItemEventArgs(System.Drawing.Graphics graphics, System.Drawing.Font font, System.Drawing.Rectangle rect, System.Int32 index, DrawItemState state, System.Drawing.Color foreColor, System.Drawing.Color backColor)
        {
            this.graphics = graphics;
            this.font = font;
            this.rect = rect;
            this.index = index;
            this.state = state;
            this.foreColor = foreColor;
            this.backColor = backColor;
        }

        /// <summary>
        /// Initializes a new instance
        /// </summary>
        public DrawItemEventArgs(System.Drawing.Graphics graphics, System.Drawing.Font font, System.Drawing.Rectangle rect, System.Int32 index, DrawItemState state)
        {
            this.graphics = graphics;
            this.font = font;
            this.rect = rect;
            this.index = index;
            this.state = state;
            this.foreColor = SystemColors.ControlText;
            this.backColor = SystemColors.Window;
        }

        /// <summary>
        /// Gets the rectangle that represents the bounds of the item that is being drawn.
        /// </summary>
        public Rectangle Bounds
        {
            get
            {
                return rect;
            }
        }

        /// <summary>
        /// Draws the background within the bounds specified in the DrawItemEventArgs constructor and with the appropriate color.
        /// </summary>
        public virtual void DrawBackground()
        {
            Brush brush;

            if (state == DrawItemState.Selected)
            {
                brush = new SolidBrush(SystemColors.Highlight);
            }
            else
            {
                brush = new SolidBrush(backColor);
            }

            Rectangle rc = new Rectangle(rect.X + 1, rect.Y, rect.Width, rect.Height - 1);

            rc.Y += 1;

            graphics.FillRectangle(brush, rc);
        }

        /// <summary>
        /// Draws the background within the bounds specified in the DrawItemEventArgs constructor and with the appropriate color.
        /// </summary>
        public virtual void DrawBackground(Color color)
        {
            Brush brush = new SolidBrush(color);
            Rectangle rc = new Rectangle(rect.X + 1, rect.Y, rect.Width, rect.Height - 1);
            rc.Y += 1;
            graphics.FillRectangle(brush, rc);
        }

        public virtual void DrawFocusRectangle()
        {
            Rectangle focusRect = rect;
            focusRect.Width--;
            focusRect.Inflate(-1, 0);
            graphics.DrawRectangle(new Pen(SystemColors.Highlight), focusRect);
        }

        public DrawItemState State
        {
            get { return state; }
        }

        public Graphics Graphics
        {
            get { return graphics; }
        }

        public int Index
        {
            get { return index; }
        }

        public System.Drawing.Font Font
        {
            get { return font; }
        }

        public Color BackColor
        {
            get { return backColor; }
        }

        public Color ForeColor
        {
            get { return foreColor; }
        }

    }

    #endregion

    #region OwnerDrawnList
    /// <summary>
    /// Summary description for OwnerDrawList.
    /// </summary>
    public abstract class OwnerDrawnList : Control
    {
        //DrawItem event
        public event DrawItemEventHandler DrawItem;

        //SelectedIndexChanged event
        public event EventHandler SelectedIndexChanged;

        //Items array
        protected ArrayList listItems;
        //Some default item setting
        private int itemHeight = 14;
        private int selectedIndex = -1;
        private int SCROLL_WIDTH = 20;

        //Bitmap for double-buffering
        private Bitmap m_bmpOffscreen;

        private int topIndex;

        private int itemWidth;

        private bool showScrollbar;
        //Vertical scroll
        protected ScrollBar vScroll;

        //Default constructor
        public OwnerDrawnList()
        {
            listItems = new ArrayList();
            //Create a ScrollBar instance
            vScroll = new VScrollBar();
            //Hide it for now
            vScroll.Hide();
            //Hookup into its ValueChanged event
            vScroll.ValueChanged += new EventHandler(vScrollcroll_ValueChanged);

            //Add Scrollbar
            this.Controls.Add(vScroll);

            SCROLL_WIDTH = vScroll.Width;

            itemWidth = this.Width;
        }        

        /// <summary>
        /// Get/set the ShowScrollbar property.  
        /// </summary>
        public bool ShowScrollbar
        {
            get
            {
                return showScrollbar;
            }
            set
            {
                showScrollbar = value;
                vScroll.Visible = showScrollbar;
            }
        }

        /// <summary>
        /// Gets or sets the index of the first visible item in the OwnerDrawnList. 
        /// </summary>
        public int TopIndex
        {
            get
            {
                return topIndex;
            }
            set
            {
                topIndex = value;
                this.Invalidate();
            }
        }


        /// <summary>
        /// Gets the items of the List.  
        /// </summary>
        public ArrayList Items
        {
            get
            {
                return listItems;
            }
        }

        /// <summary>
        /// Gets the position that has been scrolled to at this point
        /// </summary>
        public int vScrollValue
        {
            get
            {
                return vScroll.Value;
            }
        }


        /// <summary>
        /// Gets or sets a ItemHeight
        /// </summary>
        public virtual int ItemHeight
        {
            get
            {
                return this.itemHeight;
            }
            set
            {
                this.itemHeight = value;
            }
        }

        /// <summary>
        /// Raises the SelectedIndexChanged event.  
        /// </summary>
        /// <param name="e"></param>
        protected virtual void OnSelectedIndexChanged(EventArgs e)
        {
            if (this.SelectedIndexChanged != null)
                this.SelectedIndexChanged(this, e);
        }


        /// <summary>
        /// Raises the DrawItem event.  
        /// </summary>
        protected virtual void OnDrawItem(object sender, DrawItemEventArgs e)
        {
            if (this.DrawItem != null)
                this.DrawItem(sender, e);
        }


        protected override void OnResize(EventArgs e)
        {
            if (this.m_bmpOffscreen != null)
                this.m_bmpOffscreen.Dispose(); 

            if (!this.showScrollbar)
            {
                //Resize the offscreen bitmap
                this.m_bmpOffscreen = new Bitmap(this.ClientSize.Width, this.ClientSize.Height);
            }
            else
            {
                //Adjust the height
                this.Height = ((int)this.Height / itemHeight) * itemHeight;

                //How many items are visible
                int viewableItemCount = this.ClientSize.Height / this.ItemHeight;

                this.vScroll.Bounds = new Rectangle(this.ClientSize.Width - SCROLL_WIDTH,
                    0,
                    SCROLL_WIDTH,
                    this.ClientSize.Height);

                // Determine if scrollbars are needed
                if (this.listItems.Count > viewableItemCount)
                {
                    this.vScroll.Visible = true;
                    this.vScroll.LargeChange = viewableItemCount;
                    this.m_bmpOffscreen = new Bitmap(this.ClientSize.Width - SCROLL_WIDTH, this.ClientSize.Height);
                }
                else
                {
                    //showScrollbar = false;
                    this.vScroll.Visible = false;
                    this.vScroll.LargeChange = this.listItems.Count;
                    this.m_bmpOffscreen = new Bitmap(this.ClientSize.Width, this.ClientSize.Height);
                }

                this.vScroll.Maximum = this.listItems.Count - 1;
            }
        }

        //Override this for reducing a flicker
        protected override void OnPaintBackground(System.Windows.Forms.PaintEventArgs e)
        {
            //Do nothing
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            //get out if there are no items
            if (listItems.Count == 0)
                return;

            int prevSelection = selectedIndex;

            selectedIndex = this.vScroll.Value + (e.Y / this.ItemHeight);
            if (SelectedIndex >= Items.Count)
                return;

            Graphics gxTemp = this.CreateGraphics();

            if (prevSelection != -1)
                PaintItem(gxTemp, prevSelection);

            PaintItem(gxTemp, selectedIndex);

            DrawBorder(gxTemp);

            this.Focus();
        }

        /// <summary>
        /// Gets or sets the zero-based index of the currently selected item in a OwnerDrawnList.  
        /// </summary>
        public int SelectedIndex
        {
            get
            {
                return this.selectedIndex;
            }
            set
            {
                this.selectedIndex = value;
                if (this.SelectedIndexChanged != null)
                    this.SelectedIndexChanged(this, EventArgs.Empty);

                this.Invalidate();
            }
        }

        /// <summary>
        /// Ensures that the specified item is visible within the control, scrolling the contents of the control if necessary.  
        /// </summary>
        /// <param name="index">The zero-based index of the item to scroll into view.</param>
        public void EnsureVisible(int index)
        {
            if (index < this.vScroll.Value)
            {
                this.vScroll.Value = index;
                this.Refresh();
            }
            else if (index >= this.vScroll.Value + this.DrawCount)
            {
                this.vScroll.Value = index - this.DrawCount + 1;
                this.Refresh();
            }
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.Down:
                    if (this.SelectedIndex < this.vScroll.Maximum)
                    {
                        EnsureVisible(++this.SelectedIndex);
                        this.Refresh();
                    }
                    break;
                case Keys.Up:
                    if (this.SelectedIndex > this.vScroll.Minimum)
                    {
                        EnsureVisible(--this.SelectedIndex);
                        this.Refresh();
                    }
                    break;
                case Keys.PageDown:
                    this.SelectedIndex = Math.Min(this.vScroll.Maximum, this.SelectedIndex + this.DrawCount);
                    EnsureVisible(this.SelectedIndex);
                    this.Refresh();
                    break;
                case Keys.PageUp:
                    this.SelectedIndex = Math.Max(this.vScroll.Minimum, this.SelectedIndex - this.DrawCount);
                    EnsureVisible(this.SelectedIndex);
                    this.Refresh();
                    break;
                case Keys.Home:
                    this.SelectedIndex = 0;
                    EnsureVisible(this.SelectedIndex);
                    this.Refresh();
                    break;
                case Keys.End:
                    this.SelectedIndex = this.listItems.Count - 1;
                    EnsureVisible(this.SelectedIndex);
                    this.Refresh();
                    break;
            }

            base.OnKeyDown(e);
        }

        protected override void OnPaint(System.Windows.Forms.PaintEventArgs e)
        {
            //Create Graphics for off screen painting
            Graphics gxOff = Graphics.FromImage(m_bmpOffscreen);

            //Clear
            gxOff.Clear(this.BackColor);

            int drawCount = 0;

            //Get the first visible item
            if (showScrollbar)
                topIndex = vScroll.Value;
            else
                topIndex = 0;

            //Get the count of the items to draw
            if (showScrollbar)
                drawCount = DrawCount;
            else
                drawCount = listItems.Count;

            if (vScroll.Visible)
                itemWidth = this.Width - vScroll.Width;
            else
                itemWidth = this.Width;

            for (int index = topIndex; index < drawCount + topIndex; index++)
            {
                PaintItem(gxOff, index);
            }

            DrawBorder(gxOff);

            //Blit on the control's Graphics
            e.Graphics.DrawImage(m_bmpOffscreen, 0, 0);

            gxOff.Dispose();
        }

        //Helper function
        private void DrawBorder(Graphics gr)
        {
            Rectangle rc = this.ClientRectangle;
            rc.Height--;
            rc.Width--;
            //Draw border
            gr.DrawRectangle(new Pen(Color.Black), rc);

        }

        protected void PaintItem(Graphics graphics, int Index)
        {
            //			MeasureItemEventArgs measArgs = new MeasureItemEventArgs(graphics, Index, itemHeight);
            //			
            //			//Raise MeasureItemEvent
            //			if (drawMode == DrawMode.OwnerDrawVariable)
            //			{	
            //				OnMeasureItem(this, measArgs);
            //			}
            //			
            //			this.itemHeight = measArgs.ItemHeight;

            Rectangle itemRect = new Rectangle(0, (Index - topIndex) * this.itemHeight, itemWidth, this.itemHeight);

            //graphics.Clip = new Region(itemRect);

            DrawItemState state;

            //Set the appropriate selected state
            if (Index == selectedIndex)
                state = DrawItemState.Selected;
            else
                state = DrawItemState.None;

            //Prepare Args
            DrawItemEventArgs drawArgs = new DrawItemEventArgs(graphics, this.Font, itemRect, Index, state);
            //Raise drawing event for inheritors
            OnDrawItem(this, drawArgs);

        }

        // Calculate how many listItems we can draw given the height of the control.
        protected int DrawCount
        {
            get
            {
                if (this.vScroll.Value + this.vScroll.LargeChange > this.vScroll.Maximum)
                    return this.vScroll.Maximum - this.vScroll.Value + 1;
                else
                    return this.vScroll.LargeChange;
            }
        }

        //Scroll event
        private void vScrollcroll_ValueChanged(object sender, EventArgs e)
        {
            this.Refresh();
        }

        protected override void OnParentChanged(System.EventArgs e)
        {
            if (!showScrollbar)
                this.Height = listItems.Count * itemHeight;
            else //Adjust the height
            {
                this.Height = ((int)this.Height / itemHeight) * itemHeight;
            }

        }

        public void Insert(int index, object value)
        {
            Items.Insert(index, value);
        }
        /// <summary>
        ///  
        /// </summary>
        /// <param name="value"></param>
        /// <exception cref="System.NotSupportedException"></exception>
        public void Add(object value)
        {
            Items.Add(value);
        }

        public void RefreshVal()
        {
            EventArgs a = new EventArgs();
            OnResize(a);
            OnParentChanged(a);
            this.Invalidate();
            this.Refresh();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <exception cref="System.NotSupportedException"></exception>
        public void Clear()
        {
            Items.Clear();
            this.vScroll.Value = 0;
            //RefreshVal();
        }
    }

    #endregion
}
