﻿//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:2.0.50727.1433
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

namespace WineEntryClient.Properties {
    using System;
    
    
    /// <summary>
    ///   A strongly-typed resource class, for looking up localized strings, etc.
    /// </summary>
    // This class was auto-generated by the StronglyTypedResourceBuilder
    // class via a tool like ResGen or Visual Studio.
    // To add or remove a member, edit your .ResX file then rerun ResGen
    // with the /str option, or rebuild your VS project.
    [global::System.CodeDom.Compiler.GeneratedCodeAttribute("System.Resources.Tools.StronglyTypedResourceBuilder", "2.0.0.0")]
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute()]
    [global::System.Runtime.CompilerServices.CompilerGeneratedAttribute()]
    internal class Resources {
        
        private static global::System.Resources.ResourceManager resourceMan;
        
        private static global::System.Globalization.CultureInfo resourceCulture;
        
        [global::System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1811:AvoidUncalledPrivateCode")]
        internal Resources() {
        }
        
        /// <summary>
        ///   Returns the cached ResourceManager instance used by this class.
        /// </summary>
        [global::System.ComponentModel.EditorBrowsableAttribute(global::System.ComponentModel.EditorBrowsableState.Advanced)]
        internal static global::System.Resources.ResourceManager ResourceManager {
            get {
                if (object.ReferenceEquals(resourceMan, null)) {
                    global::System.Resources.ResourceManager temp = new global::System.Resources.ResourceManager("WineEntryClient.Properties.Resources", typeof(Resources).Assembly);
                    resourceMan = temp;
                }
                return resourceMan;
            }
        }
        
        /// <summary>
        ///   Overrides the current thread's CurrentUICulture property for all
        ///   resource lookups using this strongly typed resource class.
        /// </summary>
        [global::System.ComponentModel.EditorBrowsableAttribute(global::System.ComponentModel.EditorBrowsableState.Advanced)]
        internal static global::System.Globalization.CultureInfo Culture {
            get {
                return resourceCulture;
            }
            set {
                resourceCulture = value;
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to .
        /// </summary>
        internal static string emptyString {
            get {
                return ResourceManager.GetString("emptyString", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Failed..
        /// </summary>
        internal static string FailedNoReason {
            get {
                return ResourceManager.GetString("FailedNoReason", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Failed: .
        /// </summary>
        internal static string failedWithReason {
            get {
                return ResourceManager.GetString("failedWithReason", resourceCulture);
            }
        }
        
        internal static System.Drawing.Bitmap Gentaglogo {
            get {
                object obj = ResourceManager.GetObject("Gentaglogo", resourceCulture);
                return ((System.Drawing.Bitmap)(obj));
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Invalid RFID #..
        /// </summary>
        internal static string InvalidRFID {
            get {
                return ResourceManager.GetString("InvalidRFID", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Success..
        /// </summary>
        internal static string Success {
            get {
                return ResourceManager.GetString("Success", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Gentag Wine Entry Client.
        /// </summary>
        internal static string titleString {
            get {
                return ResourceManager.GetString("titleString", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Unable to enable device.
        /// </summary>
        internal static string UnableToEnableDevice {
            get {
                return ResourceManager.GetString("UnableToEnableDevice", resourceCulture);
            }
        }
        
        /// <summary>
        ///   Looks up a localized string similar to Unable to open comm to reader.
        /// </summary>
        internal static string UnableToOpenCommToReader {
            get {
                return ResourceManager.GetString("UnableToOpenCommToReader", resourceCulture);
            }
        }
    }
}
