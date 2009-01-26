import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.zip.GZIPInputStream;
import java.util.zip.GZIPOutputStream;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Document;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;


public class DramSimValid
{
  private static HashMap<String, Integer> timingParameters;
  private DramChannel[] chans;
  private ArrayList<DramCommand> commands;
  private int lengthOfWindow; //In cycles
  private String errors, outFile;
  private long currentB, totalB;
  private GZIPOutputStream gOut;
  private int errorsFound;

  public DramSimValid(String filename, String timingFile, String outFilename)
    throws IOException
  {
    errorsFound = 0;
    parseParameters(timingFile);
    System.out.println(checkParameters());

    lengthOfWindow = getLongestTiming();
    for (int i = 0; i < chans.length; i++)
      chans[i] = new DramChannel(timingParameters, lengthOfWindow);
    commands = new ArrayList<DramCommand>();
    commands.ensureCapacity(lengthOfWindow);
    outFile = outFilename;
    readTrace(filename);
  }

  public void parseParameters(String file)
  {
    timingParameters = new HashMap<String, Integer>();
    DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
    File f = new File(file);
    try
    {
      DocumentBuilder db = dbf.newDocumentBuilder();
      Document doc = db.parse(f);

      // figure out what kind of dram this is
      NodeList nl0 = doc.getElementsByTagName("dramspec");
      for (int i = 0; i < nl0.getLength(); i++)
      {
        if (nl0.item(i).getNodeName() == "dramspec")
        {
          NamedNodeMap nnm = nl0.item(i).getAttributes();
          Node n = nnm.getNamedItem("type");
          String val = n.getNodeValue();
          if (val.matches("ddr"))
          {
            timingParameters.put("tCCD", 2);
            timingParameters.put("tOST", 0);
          }
          else if (val.matches("ddr2"))
          {
            timingParameters.put("tCCD", 4);
            timingParameters.put("tOST", 5);
          }
          else if (val.matches("ddr3"))
          {
            timingParameters.put("tCCD", 8);
            timingParameters.put("tOST", 5);
          }
        }
      }

      NodeList nl = doc.getElementsByTagName("timing");
      nl = nl.item(0).getChildNodes();

      for (int i = 0; i < nl.getLength(); i++)
      {
        if (nl.item(i).getNodeName().startsWith("t") && !nl.item(i).getNodeName().startsWith("transaction"))
        {
          timingParameters.put(nl.item(i).getNodeName(), Integer.parseInt(nl.item(i).getTextContent()));
        }
      }
      nl = doc.getElementsByTagName("channels");
      chans = new DramChannel[Integer.parseInt(nl.item(0).getTextContent())];
      
    }
    catch (NumberFormatException e)
    {
      e.printStackTrace();
    }
    catch (Exception e)
    {
      e.printStackTrace();
    }

  }

  public void readTrace(String file)
    throws IOException
  {
    File fi = new File(file);
    totalB = fi.length();
    currentB = 0;
    try
    {
      FileOutputStream fOut = new FileOutputStream(outFile);
      gOut = new GZIPOutputStream(fOut);
    }
    catch (FileNotFoundException e)
    {
      e.printStackTrace();
    }
    catch (IOException e)
    {
      e.printStackTrace();
    }
    //FileInputStream fis = new FileInputStream(file);
    DramCommand dc;
    //System.out.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||");
    try
    {
      // GZIPInputStream gzStream = new GZIPInputStream(fis, 32768);
      BufferedReader gzStream = 
        new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(file))));
      //byte[] b = new byte[1];
      //String line = "";
      String line = gzStream.readLine();
      while (line != null)
      {
        if (line.startsWith("C F"))
        {
          dc = new DramCommand(line);
          //Process Compound Commands
          if (dc.getType().equals("CAS+P"))
          {
            DramCommand CAS = dc.getCAS();
            DramCommand Pre = dc.getPre();
            sendCommand(CAS);
            sendCommand(Pre);
          }
          else if (dc.getType().equals("CASW+P"))
          {
            DramCommand CAS = dc.getCAS();
            DramCommand Pre = dc.getPre();
            sendCommand(CAS);
            sendCommand(Pre);
          }
          else
          {
            sendCommand(dc);
          }
        }

        if (currentB++ % 100000 == 0)
        {
          System.out.print(currentB + "/" + totalB + "/" + errorsFound + "\r");
        }
        line = gzStream.readLine();
      }
    }
    catch (IOException ie)
    {
      System.err.println("Unable to read file");
    }
    finally
    {
      //fis.close();
      gOut.close();
    }
  }

  public void sendCommand(DramCommand com)
  {
    //System.out.println(com.getType()+ " {S,E}: {" +com.getS() + "," + com.getE() + "} s:" 
    //	+ commands.size()+ " chan:"+com.getChan());
    commands.add(0, com);
    while (commands.get(commands.size() - 1).getS() + lengthOfWindow < com.getS())
    {
      commands.remove(commands.size() - 1);
    }
    errors = "";
    errors = chans[com.getChan()].checkTiming(com);
    if (errors != null && errors.length() > 0)
    {
      errorsFound++;
    }
    try
    {
      gOut.write(errors.getBytes());
    }    
    catch (ArrayIndexOutOfBoundsException e)
    {
      e.printStackTrace();
      System.out.println(com.getChan());
    }
    catch (IOException e)
    {
      e.printStackTrace();
    }

    //System.out.print(errors);
  }

  public static int getTimingParameter(String name)
  {
    return timingParameters.get(name);
  }

  public int getLongestTiming()
  {
    //Determines the longest possible timing constraint in cycles
    int t = timingParameters.get("tRC");
    if (t < timingParameters.get("tRRD") + timingParameters.get("tFAW"))
      t = timingParameters.get("tRRD") + timingParameters.get("tFAW");
    else if (t < timingParameters.get("tRP"))
      t = timingParameters.get("tRP");
    else if (t < timingParameters.get("tRFC"))
      t = timingParameters.get("tRFC");
    else if (t < timingParameters.get("tRCD") - timingParameters.get("tAL"))
      t = timingParameters.get("tRP") - timingParameters.get("tAL");
    else if (t < Math.max(timingParameters.get("tBurst"), timingParameters.get("tCCD")))
      t = Math.max(timingParameters.get("tBurst"), timingParameters.get("tCCD"));
    else if (t < timingParameters.get("tBurst") + timingParameters.get("tRTRS"))
      t = timingParameters.get("tBurst") + timingParameters.get("tRTRS");
    else if (t < timingParameters.get("tBurst") + timingParameters.get("tCWD") + timingParameters.get("tWTR"))
      t = timingParameters.get("tBurst") + timingParameters.get("tCWD") + timingParameters.get("tWTR");
    else if (t < 
             timingParameters.get("tBurst") + timingParameters.get("tCWD") + timingParameters.get("tRTRS") - timingParameters.get("tCAS"))
      t = 
          timingParameters.get("tBurst") + timingParameters.get("tCWD") + timingParameters.get("tRTRS") - timingParameters.get("tCAS");
    else if (t < 
             timingParameters.get("tBurst") + timingParameters.get("tCAS") + timingParameters.get("tRTRS") - timingParameters.get("tCWD"))
      t = 
          timingParameters.get("tBurst") + timingParameters.get("tCAS") + timingParameters.get("tRTRS") - timingParameters.get("tCWD");
    else if (t < timingParameters.get("tBurst") + timingParameters.get("tOTS"))
      t = timingParameters.get("tBurst") + timingParameters.get("tOTS");
    else if (t < timingParameters.get("tRAS"))
      t = timingParameters.get("tRAS");
    else if (t < 
             timingParameters.get("tBurst") + timingParameters.get("tAL") + timingParameters.get("tRTP") - timingParameters.get("tCCD"))
      t = 
          timingParameters.get("tBurst") + timingParameters.get("tAL") + timingParameters.get("tRTP") - timingParameters.get("tCCD");
    else if (t < 
             timingParameters.get("tBurst") + timingParameters.get("tAL") + timingParameters.get("tCWD") + timingParameters.get("tWR"))
      t = 
          timingParameters.get("tBurst") + timingParameters.get("tAL") + timingParameters.get("tCWD") + timingParameters.get("tWR");
    return t;
  }

  public String checkParameters()
  {
    String perrors = "";
    if (timingParameters.get("tRC") < timingParameters.get("tRAS") + timingParameters.get("tRP"))
      perrors += "tRC<tRAS+tRP";

    return perrors;
  }
}
