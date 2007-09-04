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
namespace DHL_Demo.org.dyndns.crius {
    using System.Diagnostics;
    using System.Web.Services;
    using System.ComponentModel;
    using System.Web.Services.Protocols;
    using System;
    using System.Xml.Serialization;
    
    
    /// <remarks/>
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Web.Services.WebServiceBindingAttribute(Name="GetDatesWSSoapHttp", Namespace="http://datespackage/")]
    public partial class GetDatesWS : System.Web.Services.Protocols.SoapHttpClientProtocol {
        
        /// <remarks/>
        public GetDatesWS() {
            this.Url = "http://crius.dyndns.org:8888/GentagDemo/GetDatesWSSoapHttpPort";
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("http://datespackage//callHome", RequestElementName="callHomeElement", RequestNamespace="http://datespackage/", ResponseElementName="callHomeResponseElement", ResponseNamespace="http://datespackage/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
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
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("http://datespackage//checkInteraction", RequestElementName="checkInteractionElement", RequestNamespace="http://datespackage/", ResponseElementName="checkInteractionResponseElement", ResponseNamespace="http://datespackage/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("result")]
        public bool checkInteraction([System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] string ID, [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] string drugID) {
            object[] results = this.Invoke("checkInteraction", new object[] {
                        ID,
                        drugID});
            return ((bool)(results[0]));
        }
        
        /// <remarks/>
        public System.IAsyncResult BegincheckInteraction(string ID, string drugID, System.AsyncCallback callback, object asyncState) {
            return this.BeginInvoke("checkInteraction", new object[] {
                        ID,
                        drugID}, callback, asyncState);
        }
        
        /// <remarks/>
        public bool EndcheckInteraction(System.IAsyncResult asyncResult) {
            object[] results = this.EndInvoke(asyncResult);
            return ((bool)(results[0]));
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("http://datespackage//getItem", RequestElementName="getItemElement", RequestNamespace="http://datespackage/", ResponseElementName="getItemResponseElement", ResponseNamespace="http://datespackage/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("result", IsNullable=true)]
        public GetDates getItem([System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] string RFIDNum, [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] string UID, float lat, float longit) {
            object[] results = this.Invoke("getItem", new object[] {
                        RFIDNum,
                        UID,
                        lat,
                        longit});
            return ((GetDates)(results[0]));
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
        public GetDates EndgetItem(System.IAsyncResult asyncResult) {
            object[] results = this.EndInvoke(asyncResult);
            return ((GetDates)(results[0]));
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("http://datespackage//getPatientInfo", RequestElementName="getPatientInfoElement", RequestNamespace="http://datespackage/", ResponseElementName="getPatientInfoResponseElement", ResponseNamespace="http://datespackage/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("result", IsNullable=true)]
        public patientInfo getPatientInfo([System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] string String_1) {
            object[] results = this.Invoke("getPatientInfo", new object[] {
                        String_1});
            return ((patientInfo)(results[0]));
        }
        
        /// <remarks/>
        public System.IAsyncResult BegingetPatientInfo(string String_1, System.AsyncCallback callback, object asyncState) {
            return this.BeginInvoke("getPatientInfo", new object[] {
                        String_1}, callback, asyncState);
        }
        
        /// <remarks/>
        public patientInfo EndgetPatientInfo(System.IAsyncResult asyncResult) {
            object[] results = this.EndInvoke(asyncResult);
            return ((patientInfo)(results[0]));
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("http://datespackage//getPicture", RequestElementName="getPictureElement", RequestNamespace="http://datespackage/", ResponseElementName="getPictureResponseElement", ResponseNamespace="http://datespackage/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("result", DataType="base64Binary", IsNullable=true)]
        public byte[] getPicture([System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] string ID, bool isDrug) {
            object[] results = this.Invoke("getPicture", new object[] {
                        ID,
                        isDrug});
            return ((byte[])(results[0]));
        }
        
        /// <remarks/>
        public System.IAsyncResult BegingetPicture(string ID, bool isDrug, System.AsyncCallback callback, object asyncState) {
            return this.BeginInvoke("getPicture", new object[] {
                        ID,
                        isDrug}, callback, asyncState);
        }
        
        /// <remarks/>
        public byte[] EndgetPicture(System.IAsyncResult asyncResult) {
            object[] results = this.EndInvoke(asyncResult);
            return ((byte[])(results[0]));
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("http://datespackage//getRoute", RequestElementName="getRouteElement", RequestNamespace="http://datespackage/", ResponseElementName="getRouteResponseElement", ResponseNamespace="http://datespackage/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
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
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("http://datespackage//getSince", RequestElementName="getSinceElement", RequestNamespace="http://datespackage/", ResponseElementName="getSinceResponseElement", ResponseNamespace="http://datespackage/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
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
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("http://datespackage//getUniqueUIDs", RequestElementName="getUniqueUIDsElement", RequestNamespace="http://datespackage/", ResponseElementName="getUniqueUIDsResponseElement", ResponseNamespace="http://datespackage/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
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
    }
    
    /// <remarks/>
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://datespackage/")]
    public partial class GetDates {
        
        private bool authenticatedField;
        
        private string nameField;
        
        private string descField;
        
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
        [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)]
        public string name {
            get {
                return this.nameField;
            }
            set {
                this.nameField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)]
        public string desc {
            get {
                return this.descField;
            }
            set {
                this.descField = value;
            }
        }
    }
    
    /// <remarks/>
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://datespackage/")]
    public partial class patientInfo {
        
        private string descriptionField;
        
        private byte[] imageField;
        
        private string rfidNumField;
        
        private string nameField;
        
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
        public string name {
            get {
                return this.nameField;
            }
            set {
                this.nameField = value;
            }
        }
    }
}