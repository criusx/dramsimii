using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Net;

namespace DHL_Demo
{
    public partial class Browser : Form
    {
        public Browser()
        {
            InitializeComponent();
            webBrowser1.Navigate(new Uri("http://crius.dyndns.org:8888/GentagDemo/realtimeTracking2.html"));
        }

        private void menuItem1_Click(object sender, EventArgs e)
        {
            
            webBrowser1.DocumentText = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">" +
                "<html xmlns=\"http://www.w3.org/1999/xhtml\" xmlns:v=\"urn:schemas-microsoft-com:vml\">" + "<head><title>Real-time Position Monitoring</title>" +
                "<meta content=\"text/html; charset=UTF-8\" http-equiv=\"Content-Type\" />" +
                "<style type=\"text/css\">" +
                "v\\:* {" +
                "behavior:url(#default#VML);" +
                "}</style>" +
                "<link href=\"css/format.css\" media=\"screen\" rel=\"stylesheet\" type=\"text/css\" />" +
                "<script src=\"http://maps.google.com/maps?file=api&amp;v=2&amp;key=ABQIAAAAl6hrDb49xWS3d0WtSqQLMxSY_R9GqXOP67i2Qu6GZCnZlQkzHxSvai30snUGXYRnL0zku--QNC7hCQ\" type=\"text/javascript\" ></script>" +
                "<script type=\"text/javascript\">" +            
                "var map;" +
                "function init()" +
                "{" +
                "if (GBrowserIsCompatible())" +
                "{" +
                "map = new GMap2(document.getElementById(\"map\"));" +
                "map.addControl(new GMapTypeControl());" +
                "map.addControl(new GOverviewMapControl());" +
                "map.setCenter(new GLatLng(34.903708, - 79.066474), 13);                }" +
                "}" +
                "</script>" +
                "</head><body onload=\"init()\" onunload=\"GUnload()\"><div id=\"map\"></div></div></body></html>";
        }
    }
}