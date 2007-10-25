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
namespace GentagDemo.radWS {
    using System.Diagnostics;
    using System.Web.Services;
    using System.ComponentModel;
    using System.Web.Services.Protocols;
    using System;
    using System.Xml.Serialization;
    
    
    /// <remarks/>
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Web.Services.WebServiceBindingAttribute(Name="EvDemoWSSoapHttp", Namespace="http://evDemo/")]
    public partial class EvDemoWS : System.Web.Services.Protocols.SoapHttpClientProtocol {
        
        /// <remarks/>
        public EvDemoWS() {
            this.Url = "http://crius.dyndns.org:8888/GentagDemo/EvDemoWSSoapHttpPort";
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("http://evDemo//addSpectrum", RequestElementName="addSpectrumElement", RequestNamespace="http://evDemo/", ResponseElementName="addSpectrumResponseElement", ResponseNamespace="http://evDemo/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("result")]
        public bool addSpectrum([System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] spectrumInfo newSample) {
            object[] results = this.Invoke("addSpectrum", new object[] {
                        newSample});
            return ((bool)(results[0]));
        }
        
        /// <remarks/>
        public System.IAsyncResult BeginaddSpectrum(spectrumInfo newSample, System.AsyncCallback callback, object asyncState) {
            return this.BeginInvoke("addSpectrum", new object[] {
                        newSample}, callback, asyncState);
        }
        
        /// <remarks/>
        public bool EndaddSpectrum(System.IAsyncResult asyncResult) {
            object[] results = this.EndInvoke(asyncResult);
            return ((bool)(results[0]));
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("http://evDemo//getSpectrum", RequestElementName="getSpectrumElement", RequestNamespace="http://evDemo/", ResponseElementName="getSpectrumResponseElement", ResponseNamespace="http://evDemo/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("result", IsNullable=true)]
        public spectrumInfo getSpectrum([System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] string String_1) {
            object[] results = this.Invoke("getSpectrum", new object[] {
                        String_1});
            return ((spectrumInfo)(results[0]));
        }
        
        /// <remarks/>
        public System.IAsyncResult BegingetSpectrum(string String_1, System.AsyncCallback callback, object asyncState) {
            return this.BeginInvoke("getSpectrum", new object[] {
                        String_1}, callback, asyncState);
        }
        
        /// <remarks/>
        public spectrumInfo EndgetSpectrum(System.IAsyncResult asyncResult) {
            object[] results = this.EndInvoke(asyncResult);
            return ((spectrumInfo)(results[0]));
        }
    }
    
    /// <remarks/>
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://evDemo/")]
    public partial class spectrumInfo {
        
        private string rFIDNumField;
        
        private int[] dataArrayField;
        
        private bool existsField;
        
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
        [System.Xml.Serialization.XmlElementAttribute("dataArray")]
        public int[] dataArray {
            get {
                return this.dataArrayField;
            }
            set {
                this.dataArrayField = value;
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
    }
}