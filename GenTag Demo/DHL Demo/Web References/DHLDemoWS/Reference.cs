﻿//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:2.0.50727.1318
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

// 
// This source code was auto-generated by Microsoft.CompactFramework.Design.Data, Version 2.0.50727.1318.
// 
namespace DHL_Demo.DHLDemoWS {
    using System.Diagnostics;
    using System.Web.Services;
    using System.ComponentModel;
    using System.Web.Services.Protocols;
    using System;
    using System.Xml.Serialization;
    
    
    /// <remarks/>
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Web.Services.WebServiceBindingAttribute(Name="DhlDemoWSSoapHttp", Namespace="http://dhlDemo/")]
    public partial class DhlDemoWS : System.Web.Services.Protocols.SoapHttpClientProtocol {
        
        /// <remarks/>
        public DhlDemoWS() {
            this.Url = "http://crius.dyndns.org:8888/GentagDemo/DhlDemoWSSoapHttpPort";
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("", RequestNamespace="http://dhlDemo/", ResponseNamespace="http://dhlDemo/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("return", IsNullable=true)]
        public packageInfo getPackageInfo([System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] string RFIDNum) {
            object[] results = this.Invoke("getPackageInfo", new object[] {
                        RFIDNum});
            return ((packageInfo)(results[0]));
        }
        
        /// <remarks/>
        public System.IAsyncResult BegingetPackageInfo(string RFIDNum, System.AsyncCallback callback, object asyncState) {
            return this.BeginInvoke("getPackageInfo", new object[] {
                        RFIDNum}, callback, asyncState);
        }
        
        /// <remarks/>
        public packageInfo EndgetPackageInfo(System.IAsyncResult asyncResult) {
            object[] results = this.EndInvoke(asyncResult);
            return ((packageInfo)(results[0]));
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("", RequestNamespace="http://dhlDemo/", ResponseNamespace="http://dhlDemo/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("return", IsNullable=true)]
        public packageInfo scanPackage([System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] packageScan newScan) {
            object[] results = this.Invoke("scanPackage", new object[] {
                        newScan});
            return ((packageInfo)(results[0]));
        }
        
        /// <remarks/>
        public System.IAsyncResult BeginscanPackage(packageScan newScan, System.AsyncCallback callback, object asyncState) {
            return this.BeginInvoke("scanPackage", new object[] {
                        newScan}, callback, asyncState);
        }
        
        /// <remarks/>
        public packageInfo EndscanPackage(System.IAsyncResult asyncResult) {
            object[] results = this.EndInvoke(asyncResult);
            return ((packageInfo)(results[0]));
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("", RequestNamespace="http://dhlDemo/", ResponseNamespace="http://dhlDemo/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("return")]
        public bool setPackageInfo([System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] packageInfo newPackage) {
            object[] results = this.Invoke("setPackageInfo", new object[] {
                        newPackage});
            return ((bool)(results[0]));
        }
        
        /// <remarks/>
        public System.IAsyncResult BeginsetPackageInfo(packageInfo newPackage, System.AsyncCallback callback, object asyncState) {
            return this.BeginInvoke("setPackageInfo", new object[] {
                        newPackage}, callback, asyncState);
        }
        
        /// <remarks/>
        public bool EndsetPackageInfo(System.IAsyncResult asyncResult) {
            object[] results = this.EndInvoke(asyncResult);
            return ((bool)(results[0]));
        }
    }
    
    /// <remarks/>
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://dhlDemo/")]
    public partial class packageInfo {
        
        private string rFIDNumField;
        
        private string deliveryAddress2Field;
        
        private string deliveryAddressField;
        
        private string recipientPagerField;
        
        private bool existsField;
        
        private string recipientEmailField;
        
        private string trackingNumberField;
        
        private float longitudeField;
        
        private string countryField;
        
        private string stateField;
        
        private string zipField;
        
        private float latitudeField;
        
        private string cityField;
        
        private string recipientField;
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)]
        public string RFIDNum {
            get {
                return this.rFIDNumField;
            }
            set {
                this.rFIDNumField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)]
        public string deliveryAddress2 {
            get {
                return this.deliveryAddress2Field;
            }
            set {
                this.deliveryAddress2Field = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)]
        public string deliveryAddress {
            get {
                return this.deliveryAddressField;
            }
            set {
                this.deliveryAddressField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)]
        public string recipientPager {
            get {
                return this.recipientPagerField;
            }
            set {
                this.recipientPagerField = value;
            }
        }
        
        /// <remarks/>
        public bool exists {
            get {
                return this.existsField;
            }
            set {
                this.existsField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)]
        public string recipientEmail {
            get {
                return this.recipientEmailField;
            }
            set {
                this.recipientEmailField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)]
        public string trackingNumber {
            get {
                return this.trackingNumberField;
            }
            set {
                this.trackingNumberField = value;
            }
        }
        
        /// <remarks/>
        public float longitude {
            get {
                return this.longitudeField;
            }
            set {
                this.longitudeField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)]
        public string country {
            get {
                return this.countryField;
            }
            set {
                this.countryField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)]
        public string state {
            get {
                return this.stateField;
            }
            set {
                this.stateField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)]
        public string zip {
            get {
                return this.zipField;
            }
            set {
                this.zipField = value;
            }
        }
        
        /// <remarks/>
        public float latitude {
            get {
                return this.latitudeField;
            }
            set {
                this.latitudeField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)]
        public string city {
            get {
                return this.cityField;
            }
            set {
                this.cityField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)]
        public string recipient {
            get {
                return this.recipientField;
            }
            set {
                this.recipientField = value;
            }
        }
    }
    
    /// <remarks/>
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://dhlDemo/")]
    public partial class packageScan {
        
        private string rFIDNumField;
        
        private float longitudeField;
        
        private int actionTakenField;
        
        private string scannerIDField;
        
        private long scanTimeField;
        
        private float latitudeField;
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)]
        public string RFIDNum {
            get {
                return this.rFIDNumField;
            }
            set {
                this.rFIDNumField = value;
            }
        }
        
        /// <remarks/>
        public float longitude {
            get {
                return this.longitudeField;
            }
            set {
                this.longitudeField = value;
            }
        }
        
        /// <remarks/>
        public int actionTaken {
            get {
                return this.actionTakenField;
            }
            set {
                this.actionTakenField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)]
        public string scannerID {
            get {
                return this.scannerIDField;
            }
            set {
                this.scannerIDField = value;
            }
        }
        
        /// <remarks/>
        public long scanTime {
            get {
                return this.scanTimeField;
            }
            set {
                this.scanTimeField = value;
            }
        }
        
        /// <remarks/>
        public float latitude {
            get {
                return this.latitudeField;
            }
            set {
                this.latitudeField = value;
            }
        }
    }
}
