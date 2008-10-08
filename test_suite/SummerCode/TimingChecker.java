import java.io.File;
import java.io.IOException;

import java.util.HashMap;

import javax.xml.parsers.*;
import javax.xml.transform.Source;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamSource;
import javax.xml.validation.*;
import javax.xml.XMLConstants;

import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import org.xml.sax.ErrorHandler;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;


public class TimingChecker
{

  /**
   * @param args[0] The filename for the xml file to be processed
   */
  public static void main(String[] args)
  {
    String filename = args[0];

    DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
    factory.setValidating(true);
    factory.setNamespaceAware(false);
    HashMap<String, Integer> timingParameters = 
      new HashMap<String, Integer>();


    //SchemaFactory sfactory = SchemaFactory.newInstance(XMLConstants.W3C_XML_SCHEMA_NS_URI);
    //sfactory.setErrorHandler(new myErrorHandler());	

    try
    {
      //Source schemaFile = new StreamSource(new File("/home/colin/m5-dramsim/src/mem/DRAMsimII/memoryDefinitions/schema.xsd"));
      /*if(!schemaFile.exists()){
				System.out.println("schemaFile does not exist");
				System.exit(1);
			}
				Schema xmlSchema = sfactory.newSchema(schemaFile);
			Validator validator = xmlSchema.newValidator();*/

      DocumentBuilder builder = factory.newDocumentBuilder();
      Document xmlDoc = builder.parse(new File(filename));
      //validator.validate(new DOMSource(xmlDoc));

      NodeList timingParams = 
        xmlDoc.getElementsByTagName("timing").item(0).getChildNodes();

      for (int i = 0; i < timingParams.getLength(); i++)
      {
        if (timingParams.item(i).getNodeType() != Node.COMMENT_NODE && 
            !timingParams.item(i).getTextContent().isEmpty() && 
            timingParams.item(i).getNodeName().startsWith("t") && 
            !timingParams.item(i).getNodeName().startsWith("transaction"))
        {
          System.out.println(timingParams.item(i).getNodeName() + " : " + 
                             timingParams.item(i).getTextContent());
          timingParameters.put(timingParams.item(i).getNodeName(), 
                               Integer.valueOf(timingParams.item(i).getTextContent()));
        }
      }

      //check for timing parameters that don't make sense

      if (timingParameters.get("tRAS") + timingParameters.get("tRP") < 
          timingParameters.get("tRC"))
      {
        System.out.println("tRAS + tRP < tRC");
      }
      if (timingParameters.get("tRCD") + timingParameters.get("tCWD") + 
          timingParameters.get("tBurst") + timingParameters.get("tWR") < 
          timingParameters.get("tRAS"))
      {
        System.out.println("tRCD + tCWD + tBurst + tWR < tRAS");
      }
      /*if(timingParameters.get("tAL") + timingParameters.get("tCWD") + timingParameters.get("tBurst") + timingParameters.get("tWR") < timingParameters.get("tRAS")){
				System.out.println("tAL + tCWD + tBurst + tWR < tRAS");
			}
			if(timingParameters.get("tAL") + timingParameters.get("tRTP") + timingParameters.get("tBurst") - timingParameters.get("tCCD") < timingParameters.get("tRAS")){
				System.out.println("tAL + tRTP + tBurst - tCCD < tRAS");
			}*/
      if (timingParameters.get("tRCD") + timingParameters.get("tBurst") + 
          timingParameters.get("tRTP") - timingParameters.get("tCCD") < 
          timingParameters.get("tRAS"))
      {
        System.out.println("tRCD + tBurst + tRTP -tCCD < tRAS (ras to pre doesn't equal ras to cas + cas to pre");
      }

    }
    catch (FactoryConfigurationError fce)
    {
      System.err.println("Factory Configuration Error");
    }
    catch (ParserConfigurationException pce)
    {
      System.out.println("Parser Configuration Error");
    }
    catch (SAXException se)
    {
      System.out.println("SAX Exception");
    }
    catch (IOException ioe)
    {
      System.out.println("IO Exception:" + ioe.getMessage());
    }


  }

}

class myErrorHandler
  implements ErrorHandler
{

  public void error(SAXParseException arg0)
    throws SAXException
  {
    System.out.println("Error: " + arg0.getMessage());

  }

  public void fatalError(SAXParseException arg0)
    throws SAXException
  {
    System.out.println("Fatal Error: " + arg0.getMessage());
    System.exit(1);

  }

  public void warning(SAXParseException arg0)
    throws SAXException
  {
    System.out.println("Warning: " + arg0.getMessage());

  }

}
