﻿//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:2.0.50727.312
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

// 
// This source code was auto-generated by Microsoft.CompactFramework.Design.Data, Version 2.0.50727.312.
// 
namespace GentagDemo.wineWS {
    using System.Diagnostics;
    using System.Web.Services;
    using System.ComponentModel;
    using System.Web.Services.Protocols;
    using System;
    using System.Xml.Serialization;
    
    
    /// <remarks/>
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Web.Services.WebServiceBindingAttribute(Name="wineWSSoapHttp", Namespace="http://winePackage/")]
    public partial class wineWS : System.Web.Services.Protocols.SoapHttpClientProtocol {
        
        /// <remarks/>
        public wineWS() {
            this.Url = "http://crius.dyndns.org:8888/GentagDemo/wineWSSoapHttpPort";
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("http://winePackage//enterBottleInformation", RequestElementName="enterBottleInformationElement", RequestNamespace="http://winePackage/", ResponseElementName="enterBottleInformationResponseElement", ResponseNamespace="http://winePackage/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("result")]
        public bool enterBottleInformation([System.Xml.Serialization.XmlElementAttribute("arrayOfString_1", IsNullable=true)] string[] arrayOfString_1, [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] string String_2, int int_3, [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] string String_4, [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] string String_5, [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] string String_6, [System.Xml.Serialization.XmlElementAttribute(DataType="base64Binary", IsNullable=true)] byte[] arrayOfbyte_7) {
            object[] results = this.Invoke("enterBottleInformation", new object[] {
                        arrayOfString_1,
                        String_2,
                        int_3,
                        String_4,
                        String_5,
                        String_6,
                        arrayOfbyte_7});
            return ((bool)(results[0]));
        }
        
        /// <remarks/>
        public System.IAsyncResult BeginenterBottleInformation(string[] arrayOfString_1, string String_2, int int_3, string String_4, string String_5, string String_6, byte[] arrayOfbyte_7, System.AsyncCallback callback, object asyncState) {
            return this.BeginInvoke("enterBottleInformation", new object[] {
                        arrayOfString_1,
                        String_2,
                        int_3,
                        String_4,
                        String_5,
                        String_6,
                        arrayOfbyte_7}, callback, asyncState);
        }
        
        /// <remarks/>
        public bool EndenterBottleInformation(System.IAsyncResult asyncResult) {
            object[] results = this.EndInvoke(asyncResult);
            return ((bool)(results[0]));
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("http://winePackage//getWineAccessesSince", RequestElementName="getWineAccessesSinceElement", RequestNamespace="http://winePackage/", ResponseElementName="getWineAccessesSinceResponseElement", ResponseNamespace="http://winePackage/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("result", IsNullable=true)]
        public string[] getWineAccessesSince(long long_1) {
            object[] results = this.Invoke("getWineAccessesSince", new object[] {
                        long_1});
            return ((string[])(results[0]));
        }
        
        /// <remarks/>
        public System.IAsyncResult BegingetWineAccessesSince(long long_1, System.AsyncCallback callback, object asyncState) {
            return this.BeginInvoke("getWineAccessesSince", new object[] {
                        long_1}, callback, asyncState);
        }
        
        /// <remarks/>
        public string[] EndgetWineAccessesSince(System.IAsyncResult asyncResult) {
            object[] results = this.EndInvoke(asyncResult);
            return ((string[])(results[0]));
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("http://winePackage//retrieveBottleInformation", RequestElementName="retrieveBottleInformationElement", RequestNamespace="http://winePackage/", ResponseElementName="retrieveBottleInformationResponseElement", ResponseNamespace="http://winePackage/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("result", IsNullable=true)]
        public wineBottle retrieveBottleInformation([System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] string String_1, [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] string String_2, float float_3, float float_4) {
            object[] results = this.Invoke("retrieveBottleInformation", new object[] {
                        String_1,
                        String_2,
                        float_3,
                        float_4});
            return ((wineBottle)(results[0]));
        }
        
        /// <remarks/>
        public System.IAsyncResult BeginretrieveBottleInformation(string String_1, string String_2, float float_3, float float_4, System.AsyncCallback callback, object asyncState) {
            return this.BeginInvoke("retrieveBottleInformation", new object[] {
                        String_1,
                        String_2,
                        float_3,
                        float_4}, callback, asyncState);
        }
        
        /// <remarks/>
        public wineBottle EndretrieveBottleInformation(System.IAsyncResult asyncResult) {
            object[] results = this.EndInvoke(asyncResult);
            return ((wineBottle)(results[0]));
        }
    }
    
    /// <remarks/>
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://winePackage/")]
    public partial class wineBottle {
        
        private int yearField;
        
        private string typeField;
        
        private bool authenticatedField;
        
        private byte[] imageField;
        
        private int lookedUpTimesField;
        
        private bool existsField;
        
        private string vineyardField;
        
        private string originField;
        
        private string rfidNumField;
        
        private string reviewField;
        
        /// <remarks/>
        public int year {
            get {
                return this.yearField;
            }
            set {
                this.yearField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)]
        public string type {
            get {
                return this.typeField;
            }
            set {
                this.typeField = value;
            }
        }
        
        /// <remarks/>
        public bool authenticated {
            get {
                return this.authenticatedField;
            }
            set {
                this.authenticatedField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(DataType="base64Binary", IsNullable=true)]
        public byte[] image {
            get {
                return this.imageField;
            }
            set {
                this.imageField = value;
            }
        }
        
        /// <remarks/>
        public int lookedUpTimes {
            get {
                return this.lookedUpTimesField;
            }
            set {
                this.lookedUpTimesField = value;
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
        public string vineyard {
            get {
                return this.vineyardField;
            }
            set {
                this.vineyardField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)]
        public string origin {
            get {
                return this.originField;
            }
            set {
                this.originField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)]
        public string rfidNum {
            get {
                return this.rfidNumField;
            }
            set {
                this.rfidNumField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)]
        public string review {
            get {
                return this.reviewField;
            }
            set {
                this.reviewField = value;
            }
        }
    }
}
