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
namespace GentagDemo.petWS {
    using System.Diagnostics;
    using System.Web.Services;
    using System.ComponentModel;
    using System.Web.Services.Protocols;
    using System;
    using System.Xml.Serialization;
    
    
    /// <remarks/>
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Web.Services.WebServiceBindingAttribute(Name="petWSSoapHttp", Namespace="http://petPackage/")]
    public partial class petWS : System.Web.Services.Protocols.SoapHttpClientProtocol {
        
        /// <remarks/>
        public petWS() {
            this.Url = "http://crius.dyndns.org:8888/GentagDemo/petWSSoapHttpPort";
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("http://petPackage//enterPetInformation", RequestElementName="enterPetInformationElement", RequestNamespace="http://petPackage/types/", ResponseElementName="enterPetInformationResponseElement", ResponseNamespace="http://petPackage/types/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("result")]
        public bool enterPetInformation([System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] petInfo rfidNums) {
            object[] results = this.Invoke("enterPetInformation", new object[] {
                        rfidNums});
            return ((bool)(results[0]));
        }
        
        /// <remarks/>
        public System.IAsyncResult BeginenterPetInformation(petInfo rfidNums, System.AsyncCallback callback, object asyncState) {
            return this.BeginInvoke("enterPetInformation", new object[] {
                        rfidNums}, callback, asyncState);
        }
        
        /// <remarks/>
        public bool EndenterPetInformation(System.IAsyncResult asyncResult) {
            object[] results = this.EndInvoke(asyncResult);
            return ((bool)(results[0]));
        }
        
        /// <remarks/>
        [System.Web.Services.Protocols.SoapDocumentMethodAttribute("http://petPackage//retrievePetInformation", RequestElementName="retrievePetInformationElement", RequestNamespace="http://petPackage/types/", ResponseElementName="retrievePetInformationResponseElement", ResponseNamespace="http://petPackage/types/", Use=System.Web.Services.Description.SoapBindingUse.Literal, ParameterStyle=System.Web.Services.Protocols.SoapParameterStyle.Wrapped)]
        [return: System.Xml.Serialization.XmlElementAttribute("result", IsNullable=true)]
        public petInfo retrievePetInformation([System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] string String_1, [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)] string String_2, float float_3, float float_4) {
            object[] results = this.Invoke("retrievePetInformation", new object[] {
                        String_1,
                        String_2,
                        float_3,
                        float_4});
            return ((petInfo)(results[0]));
        }
        
        /// <remarks/>
        public System.IAsyncResult BeginretrievePetInformation(string String_1, string String_2, float float_3, float float_4, System.AsyncCallback callback, object asyncState) {
            return this.BeginInvoke("retrievePetInformation", new object[] {
                        String_1,
                        String_2,
                        float_3,
                        float_4}, callback, asyncState);
        }
        
        /// <remarks/>
        public petInfo EndretrievePetInformation(System.IAsyncResult asyncResult) {
            object[] results = this.EndInvoke(asyncResult);
            return ((petInfo)(results[0]));
        }
    }
    
    /// <remarks/>
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://petPackage/types/")]
    public partial class petInfo {
        
        private string contactInfoField;
        
        private string ownerField;
        
        private byte[] imageField;
        
        private string breedField;
        
        private bool existsField;
        
        private string phoneNumberField;
        
        private string rfidNumField;
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)]
        public string contactInfo {
            get {
                return this.contactInfoField;
            }
            set {
                this.contactInfoField = value;
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(IsNullable=true)]
        public string owner {
            get {
                return this.ownerField;
            }
            set {
                this.ownerField = value;
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
        public string breed {
            get {
                return this.breedField;
            }
            set {
                this.breedField = value;
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
        public string phoneNumber {
            get {
                return this.phoneNumberField;
            }
            set {
                this.phoneNumberField = value;
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
    }
}
