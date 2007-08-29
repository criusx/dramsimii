using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using System.Net;
using System.Text.RegularExpressions;

namespace DHL_Demo
{
    public enum deliveryOptions
    {
        Load = 1,
        Deliver = 2,
        Transfer = 3,
        None = 0
    }

    public partial class Checkpoint : Form
    {
        public Checkpoint()
        {
            InitializeComponent();
        }

        public deliveryOptions whichDeliveryOption
        {
            get
            {
                if (radioButton1.Checked)
                    return deliveryOptions.Load;
                else if (radioButton2.Checked)
                    return deliveryOptions.Deliver;
                else if (radioButton3.Checked)
                    return deliveryOptions.Transfer;
                else
                    return deliveryOptions.None;
            }

        }

        private string rfidNum;

        public void setInfo(string email, string streetAddress, string address2, string city, string state, string zip, string RFIDNum, string recipient, string pager)
        {
            // first set the ones that will not be looked up
            trackingLabel.Text = "TBD";
            emailLabel.Text = email;
            recipientLabel.Text = recipient;
            address2Label.Text = address2;
            rfidNum = RFIDNum;
            pagerLabel.Text = pager;

            // build the query that will be sent to the Yahoo! service
            StringBuilder url = new StringBuilder(DHL_Demo.Properties.Resources.apiURL);
            url.Append("?appid=");
            url.Append(DHL_Demo.Properties.Resources.appID);

            if (streetAddress.Length > 0)
            {
                url.Append("&street=");
                url.Append(streetAddress);
            }
            if (city.Length > 0)
            {
                url.Append("&city=");
                url.Append(city);
            }
            if (state.Length == 2)
            {
                url.Append("&state=");
                url.Append(state);
            }
            if (zip.Length >= 5)
            {
                url.Append("&zip=");
                url.Append(zip);
            }
            url = url.Replace(" ", "+");  // Yahoo example shows + sign instead of spaces.

            try
            {
                XmlReader reader = new XmlTextReader(url.ToString());
                
                XmlDocument xDoc = new XmlDocument();
                xDoc.Load(reader);

                XmlNamespaceManager xMngr = new XmlNamespaceManager(xDoc.NameTable);
                xMngr.AddNamespace("yahoo", "urn:yahoo:maps");


                try  /*  STREET */
                {
                    XmlNode xStreet;
                    xStreet = xDoc.SelectSingleNode("//yahoo:Address", xMngr);
                    address1Label.Text = xStreet.InnerText;
                }
                catch
                {
                    address1Label.Text = "";
                }

                try /* CITY */
                {
                    XmlNode xCity = xDoc.SelectSingleNode("//yahoo:City", xMngr);
                    cityLabel.Text = xCity.InnerText;
                }
                catch
                {
                    cityLabel.Text = "";
                }

                try /* STATE */
                {
                    XmlNode xStateCode = xDoc.SelectSingleNode("//yahoo:State", xMngr);
                    stateLabel.Text = xStateCode.InnerText;
                }
                catch
                {
                    stateLabel.Text = "";
                }
                try /* ZIPCODE */
                {
                    XmlNode xZipCode = xDoc.SelectSingleNode("//yahoo:Zip", xMngr);
                    zipcodeLabel.Text = xZipCode.InnerText;
                }
                catch
                {
                    zipcodeLabel.Text = "";
                }

                try /* COUNTRY */
                {
                    XmlNode xZipCode = xDoc.SelectSingleNode("//yahoo:Country", xMngr);
                    countryLabel.Text = xZipCode.InnerText;
                }
                catch
                {
                    countryLabel.Text = "";
                }

                try /* LATITUDE , LONGITUDE, PRECISION */
                {
                    XmlNode xLatitude = xDoc.SelectSingleNode("//yahoo:Latitude", xMngr);
                    latitudeLabel.Text = xLatitude.InnerText;
                    latitude = float.Parse(xLatitude.InnerText);
                    XmlNode xLongitude = xDoc.SelectSingleNode("//yahoo:Longitude", xMngr);
                    longitudeLabel.Text = xLongitude.InnerText;
                    longitude = float.Parse(xLongitude.InnerText);
                    XmlNode xPrecision = xDoc.SelectSingleNode("//yahoo:Result/@precision", xMngr);
                    //precison = xPrecision.InnerText;
                }
                catch
                {
                    latitudeLabel.Text = "";
                    longitudeLabel.Text = "";
                    //precison = "";
                }

                try  // warning is only returned if address unclear
                {
                    XmlNode xWarning = xDoc.SelectSingleNode("//yahoo:Result/@warning", xMngr);
                    string warning = xWarning.InnerText;
                    // Yahoo! embeds the depreciated <b> tag in this result (yuck!), we need to remove it.
                    warning = Regex.Replace(warning, @"<(.|\n)*?>", string.Empty);
                }
                catch
                {
                    //warning = "";
                }
            }
            catch (WebException)
            {
                MessageBox.Show("failed lookup");
            }
        }

        public void setMovementVisibility(bool value)
        {
            radioButton1.Visible = radioButton2.Visible = radioButton3.Visible = value;
        }

        private void radioButton_Click(object sender, EventArgs e)
        {
            ((RadioButton)sender).Checked = true;
            if (sender != radioButton1)
                radioButton1.Checked = false;
            if (sender != radioButton2)
                radioButton2.Checked = false;
            if (sender != radioButton3)
                radioButton3.Checked = false;
        }

        private float latitude;
        private float longitude;

        public DHLDemoWS.packageInfo getPackageInfo()
        {
            DHLDemoWS.packageInfo pI = new DHLDemoWS.packageInfo();
            pI.trackingNumber = trackingLabel.Text;
            pI.deliveryAddress = address1Label.Text;
            pI.deliveryAddress2 = address2Label.Text;
            pI.recipientEmail = emailLabel.Text;
            pI.recipientPager = pagerLabel.Text;
            pI.recipient = recipientLabel.Text;
            pI.RFIDNum = rfidNum;
            pI.city = cityLabel.Text;
            pI.state = stateLabel.Text;
            pI.zip = zipcodeLabel.Text;
            pI.latitude = latitude;
            pI.longitude = longitude;
            return pI;
        }
    }
}