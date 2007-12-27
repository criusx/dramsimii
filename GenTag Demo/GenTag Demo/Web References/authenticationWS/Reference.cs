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
namespace GentagDemo.authenticationWS {
    using System.Diagnostics;
    using System.Web.Services;
    using System.ComponentModel;
    using System.Web.Services.Protocols;
    using System;
    using System.Xml.Serialization;
    
    
    /// <remarks/>
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Web.Services.WebServiceBindingAttribute(Name="AuthenticationWebServiceSoapHttp", Namespace="http://authenticationPackage/")]
    public partial class AuthenticationWebService : System.Web.Services.Protocols.SoapHttpClientProtocol {
        
        /// <remarks/>
        public AuthenticationWebService() {
            this.Url = "http://crius.dyndns.org:8888/GentagDemo/AuthenticationWebServiceSoapHttpPort";
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("http://authenticationPackage//callHome", RequestElementName="callHomeElement", RequestNamespace="http://authenticationPackage/types/", ResponseElementName="callHomeResponseElement", ResponseNamespace="http://authenticationPackage/types/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("result")]
        public bool callHome([System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] string UID, [System.Xml.Serialization.XmlElementAttribute("lat")] float[] lat, [System.Xml.Serialization.XmlElementAttribute("longit")] float[] longit, [System.Xml.Serialization.XmlElementAttribute("timeSinceLastReading")] long[] timeSinceLastReading, [System.Xml.Serialization.XmlElementAttribute("reportedTime")] long[] reportedTime, [System.Xml.Serialization.XmlElementAttribute("bearing")] float[] bearing, [System.Xml.Serialization.XmlElementAttribute("speed")] float[] speed, [System.Xml.Serialization.XmlElementAttribute("elevation")] int[] elevation) {
            object[] results = this.Invoke("callHome", new object[] {
                        UID,
                        lat,
                        longit,
                        timeSinceLastReading,
                        reportedTime,
                        bearing,
                        speed,
                        elevation});
            return ((bool)(results[0]));
        }
        
        /// <remarks/>
        public System.IAsyncResult BegincallHome(string UID, float[] lat, float[] longit, long[] timeSinceLastReading, long[] reportedTime, float[] bearing, float[] speed, int[] elevation, System.AsyncCallback callback, object asyncState) {
            return this.BeginInvoke("callHome", new object[] {
                        UID,
                        lat,
                        longit,
                        timeSinceLastReading,
                        reportedTime,
                        bearing,
                        speed,
                        elevation}, callback, asyncState);
        }
        
        /// <remarks/>
        public bool EndcallHome(System.IAsyncResult asyncResult) {
            object[] results = this.EndInvoke(asyncResult);
            return ((bool)(results[0]));
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("http://authenticationPackage//getItem", RequestElementName="getItemElement", RequestNamespace="http://authenticationPackage/types/", ResponseElementName="getItemResponseElement", ResponseNamespace="http://authenticationPackage/types/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("result", IsNullable=true)]
        public itemInfo getItem([System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] string RFIDNum, [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] string UID, float lat, float longit) {
            object[] results = this.Invoke("getItem", new object[] {
                        RFIDNum,
                        UID,
                        lat,
                        longit});
            return ((itemInfo)(results[0]));
        }
        
        /// <remarks/>
        public System.IAsyncResult BegingetItem(string RFIDNum, string UID, float lat, float longit, System.AsyncCallback callback, object asyncState) {
            return this.BeginInvoke("getItem", new object[] {
                        RFIDNum,
                        UID,
                        lat,
                        longit}, callback, asyncState);
        }
        
        /// <remarks/>
        public itemInfo EndgetItem(System.IAsyncResult asyncResult) {
            object[] results = this.EndInvoke(asyncResult);
            return ((itemInfo)(results[0]));
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("http://authenticationPackage//getRoute", RequestElementName="getRouteElement", RequestNamespace="http://authenticationPackage/types/", ResponseElementName="getRouteResponseElement", ResponseNamespace="http://authenticationPackage/types/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("result", IsNullable=true)]
        public string[] getRoute([System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] string RFIDNum, long sinceWhen) {
            object[] results = this.Invoke("getRoute", new object[] {
                        RFIDNum,
                        sinceWhen});
            return ((string[])(results[0]));
        }
        
        /// <remarks/>
        public System.IAsyncResult BegingetRoute(string RFIDNum, long sinceWhen, System.AsyncCallback callback, object asyncState) {
            return this.BeginInvoke("getRoute", new object[] {
                        RFIDNum,
                        sinceWhen}, callback, asyncState);
        }
        
        /// <remarks/>
        public string[] EndgetRoute(System.IAsyncResult asyncResult) {
            object[] results = this.EndInvoke(asyncResult);
            return ((string[])(results[0]));
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("http://authenticationPackage//getSince", RequestElementName="getSinceElement", RequestNamespace="http://authenticationPackage/types/", ResponseElementName="getSinceResponseElement", ResponseNamespace="http://authenticationPackage/types/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("result", IsNullable=true)]
        public string[] getSince(long timestamp) {
            object[] results = this.Invoke("getSince", new object[] {
                        timestamp});
            return ((string[])(results[0]));
        }
        
        /// <remarks/>
        public System.IAsyncResult BegingetSince(long timestamp, System.AsyncCallback callback, object asyncState) {
            return this.BeginInvoke("getSince", new object[] {
                        timestamp}, callback, asyncState);
        }
        
        /// <remarks/>
        public string[] EndgetSince(System.IAsyncResult asyncResult) {
            object[] results = this.EndInvoke(asyncResult);
            return ((string[])(results[0]));
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("http://authenticationPackage//getUniqueUIDs", RequestElementName="getUniqueUIDsElement", RequestNamespace="http://authenticationPackage/types/", ResponseElementName="getUniqueUIDsResponseElement", ResponseNamespace="http://authenticationPackage/types/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("result", IsNullable=true)]
        public string[] getUniqueUIDs() {
            object[] results = this.Invoke("getUniqueUIDs", new object[0]);
            return ((string[])(results[0]));
        }
        
        /// <remarks/>
        public System.IAsyncResult BegingetUniqueUIDs(System.AsyncCallback callback, object asyncState) {
            return this.BeginInvoke("getUniqueUIDs", new object[0], callback, asyncState);
        }
        
        /// <remarks/>
        public string[] EndgetUniqueUIDs(System.IAsyncResult asyncResult) {
            object[] results = this.EndInvoke(asyncResult);
            return ((string[])(results[0]));
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("http://authenticationPackage//setItem", RequestElementName="setItemElement", RequestNamespace="http://authenticationPackage/types/", ResponseElementName="setItemResponseElement", ResponseNamespace="http://authenticationPackage/types/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("result")]
        public bool setItem([System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] itemInfo newItem, [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] string UID) {
            object[] results = this.Invoke("setItem", new object[] {
                        newItem,
                        UID});
            return ((bool)(results[0]));
        }
        
        /// <remarks/>
        public System.IAsyncResult BeginsetItem(itemInfo newItem, string UID, System.AsyncCallback callback, object asyncState) {
            return this.BeginInvoke("setItem", new object[] {
                        newItem,
                        UID}, callback, asyncState);
        }
        
        /// <remarks/>
        public bool EndsetItem(System.IAsyncResult asyncResult) {
            object[] results = this.EndInvoke(asyncResult);
            return ((bool)(results[0]));
        }
    }
    
    /// <remarks/>
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://authenticationPackage/types/")]
    public partial class itemInfo {
        
        private string rFIDNumField;
        
        private string descriptionField;
        
        private bool authenticatedField;
        
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
        public string description {
            get {
                return this.descriptionField;
            }
            set {
                this.descriptionField = value;
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
    }
}
