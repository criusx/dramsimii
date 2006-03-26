//==========================================================================================
//
//	WJ.MPR.Util.Settings
//	Copyright (c) 2005, WJ Communications, Inc.
//
//==========================================================================================
using System;
using System.Collections;
using System.Xml;
using System.IO;
using System.Diagnostics;

namespace WJ.MPR.Util
{
	/// <summary>
	///	Read and write settings to an XML .config file. Does not use 
	///	ConfigurationSettings.AppSettings since it's not supported on .NET Compact Framework. 
	///	
	///	Uses same schema as app.config file. Example:
	///
	///	NOTE: [] in example stand for less than and greater than signs in real XML...
	///	
	///	[configuration]
	///	[appSettings]
	///	[add key="Name" value="Live Oak" /]
	///	[add key="LogEvents" value="True" /]
	///	[/appSettings]
	///	[/configuration]
	///	
	///		Default settings file name is the same as app.config, 
	///		appends .config to the end of the assembly name. Example:
	///	
	///	{appname.exe}.config
	///
	/// </summary>
	public class Settings
	{
		// internal fields
		Hashtable _list = new Hashtable();
		string _filePath = "";
		bool _autoWrite = true;
		string[,] _defaultValues;

		// properties
		
		/// <summary>
		/// Specifies if the settings file is updated whenever a value 
		/// is set. If false, you need to call Write to update the 
		/// underlying settings file.
		/// </summary>
		public bool AutoWrite
		{
			get { return _autoWrite; }
			set { _autoWrite = value; }
		}
		
		/// <summary>
		/// Full path to settings file.
		/// </summary>
		public string FilePath
		{
			get { return _filePath; }
			set { _filePath = value; }
		}

		/// <summary>
		/// Default constructor. 
		/// </summary>
		public Settings(string FilePath)
		{
			// get full path to file
//			_filePath = GetFilePath();
			_filePath = FilePath;
			
			// populate list with settings from file
			Read();
		}

		/// <summary>
		/// Constructor. Pass in an array of default values.
		/// Sample usage that passes in default values:
		///
		///		string[,] values = 
		///		{
		///			{"Name", "Live Oak"},
		///			{"LogEvents", "True"}
		///		}
		///
		///		Settings settings = new Settings(values);
		/// </summary>
		public Settings(string FilePath, string[,] defaultValues)
		{
			// store default values, use later when populate list
			_defaultValues = defaultValues;

			// get full path to file
//			_filePath = GetFilePath();
			_filePath = FilePath;

			// populate list with settings from file
			Read();
		}
		

		// public methods
		
		/// <summary>
		/// Set setting value. Update underlying file if AutoUpdate is true.
		/// </summary>
		public void SetValue(string key, object value)
		{
			// update internal list
			_list[key] = value;
			
			// update settings file
			if (_autoWrite)
				Write();
		}

		/// <summary>
		/// Return specified settings as string.
		/// </summary>
		public string GetString(string key)
		{
			object result = _list[key];
			return (result == null) ? "" : result.ToString();
		}
		
		/// <summary>
		/// Return specified settings as integer.
		/// </summary>
		public int GetInt(string key)
		{
			string result = GetString(key);
			return (result == "") ? 0 : Convert.ToInt32(result);
		}

		/// <summary>
		/// Return specified settings as boolean.
		/// </summary>
		public bool GetBool(string key)
		{
			string result = GetString(key);
			return (result == "") ? false : Convert.ToBoolean(result);
		}
		
		/// <summary>
		/// Read settings file.
		/// </summary>
		public void Read()
		{
			// first remove all items from list
			 _list.Clear();
			
			// next, populate list with default values
			for (int i=0; i < _defaultValues.GetLength(0); i++)
				_list[_defaultValues[i,0]] = _defaultValues[i,1];
		
			// last, populate list with items from file
		
			// open settings file
			if (File.Exists(_filePath))
			{
				XmlTextReader reader = new XmlTextReader(_filePath);

				// go through file and read the xml file and 
				// populate internal list with 'add' elements
				while (reader.Read())
				{
					if ((reader.NodeType == XmlNodeType.Element) && (reader.Name == "add"))
						_list[reader.GetAttribute("key")] = reader.GetAttribute("value");
				}
				reader.Close();
			}
		}
		
		/// <summary>
		/// Write settings to the .config file.
		/// </summary>
		public void Write()
		{
			// header elements
			using (StreamWriter writer = File.CreateText(_filePath))
			{
				writer.WriteLine("<configuration>");
				writer.WriteLine("\t<appSettings>");

				// go through internal list and create 'add' element for each item
				IDictionaryEnumerator enumerator = _list.GetEnumerator();
				while (enumerator.MoveNext())
				{
					writer.WriteLine("\t\t<add key=\"{0}\" value=\"{1}\" />",
						enumerator.Key.ToString(),
						enumerator.Value);
				}

				// footer elements
				writer.WriteLine("\t</appSettings>");
				writer.WriteLine("</configuration>");

				writer.Close();
			}
		}

		/// <summary>
		/// Overwrite settings with default values.
		/// </summary>
		public void RestoreDefaults()
		{
			try
		{
				// easiest way is to delete the file and
				// repopulate internal list with defaults
				File.Delete(FilePath);
				Read();
			}
			catch (Exception ex)
			{
				Debug.WriteLine(ex.Message);
			}
		}


		//
		// private methods
		//
		
		/// <summary>
		/// Return full path to settings file. Appends .config to the assembly name.
		/// </summary>
		public string GetFilePath()
		{
			#if !FULL_FRAMEWORK
				return System.Reflection.Assembly.GetExecutingAssembly().GetName().CodeBase + ".config";
			#else
//				return System.Windows.Forms.Application.CommonAppDataPath + ".config";
				return System.Windows.Forms.Application.ExecutablePath + ".config";
			#endif
		}
	}
}
