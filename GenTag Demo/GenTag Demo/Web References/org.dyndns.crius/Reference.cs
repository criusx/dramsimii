﻿//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:2.0.50727.42
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

// 
// This source code was auto-generated by Microsoft.CompactFramework.Design.Data, Version 2.0.50727.42.
// 
namespace GenTag_Demo.org.dyndns.crius {
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
            this.Url = "http://crius.dyndns.org:8888/JavaWebService-GetDates-context-root/GetDatesWSSoapH" +
                "ttpPort";
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("", RequestNamespace="http://datespackage/", ResponseNamespace="http://datespackage/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("return", IsNullable=true)]
        public System.Nullable<System.DateTime> getDate() {
            object[] results = this.Invoke("getDate", new object[0]);
            return ((System.Nullable<System.DateTime>)(results[0]));
        }
        
        /// <remarks/>
        public System.IAsyncResult BegingetDate(System.AsyncCallback callback, object asyncState) {
            return this.BeginInvoke("getDate", new object[0], callback, asyncState);
        }
        
        /// <remarks/>
        public System.Nullable<System.DateTime> EndgetDate(System.IAsyncResult asyncResult) {
            object[] results = this.EndInvoke(asyncResult);
            return ((System.Nullable<System.DateTime>)(results[0]));
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("", RequestNamespace="http://datespackage/", ResponseNamespace="http://datespackage/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("return", IsNullable=true)]
        public System.Nullable<System.DateTime> getDateHence(int daysHence) {
            object[] results = this.Invoke("getDateHence", new object[] {
                        daysHence});
            return ((System.Nullable<System.DateTime>)(results[0]));
        }
        
        /// <remarks/>
        public System.IAsyncResult BegingetDateHence(int daysHence, System.AsyncCallback callback, object asyncState) {
            return this.BeginInvoke("getDateHence", new object[] {
                        daysHence}, callback, asyncState);
        }
        
        /// <remarks/>
        public System.Nullable<System.DateTime> EndgetDateHence(System.IAsyncResult asyncResult) {
            object[] results = this.EndInvoke(asyncResult);
            return ((System.Nullable<System.DateTime>)(results[0]));
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("", RequestNamespace="http://datespackage/", ResponseNamespace="http://datespackage/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("return", IsNullable=true)]
        public string getDayOfWeek(int day) {
            object[] results = this.Invoke("getDayOfWeek", new object[] {
                        day});
            return ((string)(results[0]));
        }
        
        /// <remarks/>
        public System.IAsyncResult BegingetDayOfWeek(int day, System.AsyncCallback callback, object asyncState) {
            return this.BeginInvoke("getDayOfWeek", new object[] {
                        day}, callback, asyncState);
        }
        
        /// <remarks/>
        public string EndgetDayOfWeek(System.IAsyncResult asyncResult) {
            object[] results = this.EndInvoke(asyncResult);
            return ((string)(results[0]));
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("", RequestNamespace="http://datespackage/", ResponseNamespace="http://datespackage/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("return", IsNullable=true)]
        public string getDescription([System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] string RFIDNum) {
            object[] results = this.Invoke("getDescription", new object[] {
                        RFIDNum});
            return ((string)(results[0]));
        }
        
        /// <remarks/>
        public System.IAsyncResult BegingetDescription(string RFIDNum, System.AsyncCallback callback, object asyncState) {
            return this.BeginInvoke("getDescription", new object[] {
                        RFIDNum}, callback, asyncState);
        }
        
        /// <remarks/>
        public string EndgetDescription(System.IAsyncResult asyncResult) {
            object[] results = this.EndInvoke(asyncResult);
            return ((string)(results[0]));
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("", RequestNamespace="http://datespackage/", ResponseNamespace="http://datespackage/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("return", IsNullable=true)]
        public string[] getTestStrings() {
            object[] results = this.Invoke("getTestStrings", new object[0]);
            return ((string[])(results[0]));
        }
        
        /// <remarks/>
        public System.IAsyncResult BegingetTestStrings(System.AsyncCallback callback, object asyncState) {
            return this.BeginInvoke("getTestStrings", new object[0], callback, asyncState);
        }
        
        /// <remarks/>
        public string[] EndgetTestStrings(System.IAsyncResult asyncResult) {
            object[] results = this.EndInvoke(asyncResult);
            return ((string[])(results[0]));
        }
    }
}
