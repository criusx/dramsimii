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
// This source code was auto-generated by Microsoft.VSDesigner, Version 2.0.50727.312.
// 
#pragma warning disable 1591

namespace WineEntryClient.org.dyndns.crius {
    using System.Diagnostics;
    using System.Web.Services;
    using System.ComponentModel;
    using System.Web.Services.Protocols;
    using System;
    using System.Xml.Serialization;
    
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Web.Services", "2.0.50727.312")]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Web.Services.WebServiceBindingAttribute(Name="wineWSSoapHttp", Namespace="http://winePackage/")]
    public partial class wineWS : System.Web.Services.Protocols.SoapHttpClientProtocol {
        
        private System.Threading.SendOrPostCallback enterBottleInformationOperationCompleted;
        
        private System.Threading.SendOrPostCallback retrieveBottleInformationOperationCompleted;
        
        private bool useDefaultCredentialsSetExplicitly;
        
        /// <remarks/>
        public wineWS() {
            this.Url = global::WineEntryClient.Properties.Settings.Default.WineEntryClient_org_dyndns_crius_wineWS;
            if ((this.IsLocalFileSystemWebService(this.Url) == true)) {
                this.UseDefaultCredentials = true;
                this.useDefaultCredentialsSetExplicitly = false;
            }
            else {
                this.useDefaultCredentialsSetExplicitly = true;
            }
        }
        
        public new string Url {
            get {
                return base.Url;
            }
            set {
                if ((((this.IsLocalFileSystemWebService(base.Url) == true) 
                            && (this.useDefaultCredentialsSetExplicitly == false)) 
                            && (this.IsLocalFileSystemWebService(value) == false))) {
                    base.UseDefaultCredentials = false;
                }
                base.Url = value;
            }
        }
        
        public new bool UseDefaultCredentials {
            get {
                return base.UseDefaultCredentials;
            }
            set {
                base.UseDefaultCredentials = value;
                this.useDefaultCredentialsSetExplicitly = true;
            }
        }
        
        /// <remarks/>
        public event enterBottleInformationCompletedEventHandler enterBottleInformationCompleted;
        
        /// <remarks/>
        public event retrieveBottleInformationCompletedEventHandler retrieveBottleInformationCompleted;
        
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
        public void enterBottleInformationAsync(string[] arrayOfString_1, string String_2, int int_3, string String_4, string String_5, string String_6, byte[] arrayOfbyte_7) {
            this.enterBottleInformationAsync(arrayOfString_1, String_2, int_3, String_4, String_5, String_6, arrayOfbyte_7, null);
        }
        
        /// <remarks/>
        public void enterBottleInformationAsync(string[] arrayOfString_1, string String_2, int int_3, string String_4, string String_5, string String_6, byte[] arrayOfbyte_7, object userState) {
            if ((this.enterBottleInformationOperationCompleted == null)) {
                this.enterBottleInformationOperationCompleted = new System.Threading.SendOrPostCallback(this.OnenterBottleInformationOperationCompleted);
            }
            this.InvokeAsync("enterBottleInformation", new object[] {
                        arrayOfString_1,
                        String_2,
                        int_3,
                        String_4,
                        String_5,
                        String_6,
                        arrayOfbyte_7}, this.enterBottleInformationOperationCompleted, userState);
        }
        
        private void OnenterBottleInformationOperationCompleted(object arg) {
            if ((this.enterBottleInformationCompleted != null)) {
                System.Web.Services.Protocols.InvokeCompletedEventArgs invokeArgs = ((System.Web.Services.Protocols.InvokeCompletedEventArgs)(arg));
                this.enterBottleInformationCompleted(this, new enterBottleInformationCompletedEventArgs(invokeArgs.Results, invokeArgs.Error, invokeArgs.Cancelled, invokeArgs.UserState));
            }
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("http://winePackage//retrieveBottleInformation", RequestElementName="retrieveBottleInformationElement", RequestNamespace="http://winePackage/", ResponseElementName="retrieveBottleInformationResponseElement", ResponseNamespace="http://winePackage/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("result", IsNullable=true)]
        public wineBottle[] retrieveBottleInformation([System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] string String_1, [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] string String_2, float float_3, float float_4) {
            object[] results = this.Invoke("retrieveBottleInformation", new object[] {
                        String_1,
                        String_2,
                        float_3,
                        float_4});
            return ((wineBottle[])(results[0]));
        }
        
        /// <remarks/>
        public void retrieveBottleInformationAsync(string String_1, string String_2, float float_3, float float_4) {
            this.retrieveBottleInformationAsync(String_1, String_2, float_3, float_4, null);
        }
        
        /// <remarks/>
        public void retrieveBottleInformationAsync(string String_1, string String_2, float float_3, float float_4, object userState) {
            if ((this.retrieveBottleInformationOperationCompleted == null)) {
                this.retrieveBottleInformationOperationCompleted = new System.Threading.SendOrPostCallback(this.OnretrieveBottleInformationOperationCompleted);
            }
            this.InvokeAsync("retrieveBottleInformation", new object[] {
                        String_1,
                        String_2,
                        float_3,
                        float_4}, this.retrieveBottleInformationOperationCompleted, userState);
        }
        
        private void OnretrieveBottleInformationOperationCompleted(object arg) {
            if ((this.retrieveBottleInformationCompleted != null)) {
                System.Web.Services.Protocols.InvokeCompletedEventArgs invokeArgs = ((System.Web.Services.Protocols.InvokeCompletedEventArgs)(arg));
                this.retrieveBottleInformationCompleted(this, new retrieveBottleInformationCompletedEventArgs(invokeArgs.Results, invokeArgs.Error, invokeArgs.Cancelled, invokeArgs.UserState));
            }
        }
        
        /// <remarks/>
        public new void CancelAsync(object userState) {
            base.CancelAsync(userState);
        }
        
        private bool IsLocalFileSystemWebService(string url) {
            if (((url == null) 
                        || (url == string.Empty))) {
                return false;
            }
            System.Uri wsUri = new System.Uri(url);
            if (((wsUri.Port >= 1024) 
                        && (string.Compare(wsUri.Host, "localHost", System.StringComparison.OrdinalIgnoreCase) == 0))) {
                return true;
            }
            return false;
        }
    }
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "2.0.50727.312")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://winePackage/")]
    public partial class wineBottle {
        
        private string typeField;
        
        private byte[] imageField;
        
        private string vineyardField;
        
        private string originField;
        
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
    }
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Web.Services", "2.0.50727.312")]
    public delegate void enterBottleInformationCompletedEventHandler(object sender, enterBottleInformationCompletedEventArgs e);
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Web.Services", "2.0.50727.312")]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    public partial class enterBottleInformationCompletedEventArgs : System.ComponentModel.AsyncCompletedEventArgs {
        
        private object[] results;
        
        internal enterBottleInformationCompletedEventArgs(object[] results, System.Exception exception, bool cancelled, object userState) : 
                base(exception, cancelled, userState) {
            this.results = results;
        }
        
        /// <remarks/>
        public bool Result {
            get {
                this.RaiseExceptionIfNecessary();
                return ((bool)(this.results[0]));
            }
        }
    }
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Web.Services", "2.0.50727.312")]
    public delegate void retrieveBottleInformationCompletedEventHandler(object sender, retrieveBottleInformationCompletedEventArgs e);
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Web.Services", "2.0.50727.312")]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    public partial class retrieveBottleInformationCompletedEventArgs : System.ComponentModel.AsyncCompletedEventArgs {
        
        private object[] results;
        
        internal retrieveBottleInformationCompletedEventArgs(object[] results, System.Exception exception, bool cancelled, object userState) : 
                base(exception, cancelled, userState) {
            this.results = results;
        }
        
        /// <remarks/>
        public wineBottle[] Result {
            get {
                this.RaiseExceptionIfNecessary();
                return ((wineBottle[])(this.results[0]));
            }
        }
    }
}

#pragma warning restore 1591