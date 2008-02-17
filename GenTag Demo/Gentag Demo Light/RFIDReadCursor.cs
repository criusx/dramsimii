using System.Windows.Forms;
using System.Drawing;

namespace GentagDemo
{
    public partial class RFIDReadCursor : Control
    {
        public RFIDReadCursor()
        {
            InitializeComponent();
        }

        protected override void OnPaint(PaintEventArgs pe)
        {
            // TODO: Add custom paint code here
            
            pe.Graphics.DrawImage(Image.FromHbitmap(GentagDemo.Properties.Resources.RadioAnimation0.GetHbitmap()),0,0);
            // Calling the base class OnPaint
            base.OnPaint(pe);
        }
    }
}
