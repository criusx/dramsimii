package authenticationPackage;

import java.sql.Connection;

import java.sql.SQLException;

import java.util.Calendar;
import java.util.GregorianCalendar;

import javax.jws.WebMethod;
import javax.jws.WebService;

import java.sql.*;

import oracle.jdbc.pool.OracleDataSource;
import oracle.jdbc.driver.*;

import java.io.*;

import oracle.sql.*;

import org.xml.sax.SAXException;

import com.keithpower.gekmlib.*;

import java.util.ArrayList;
import java.util.List;

import org.apache.commons.digester.Digester;

import winepackage.wineBottle;


@WebService(name = "AuthenticationWebService", serviceName = "AuthenticationWebService")
public class authenticationService
{
  public authenticationService()
  {
  }

  private static final String connectString = 
    "jdbc:oracle:thin:rfid/rfid2006@gentag-server1:1521:gentagdb";


  @WebMethod
  public String[] getRoute(String RFIDNum, long sinceWhen)
  {
    OracleDataSource ods;
    ArrayList results = new ArrayList();
    try
    {
      ods = new OracleDataSource();
      ods.setURL(connectString);
      Connection conn = ods.getConnection();
      Timestamp ts = new Timestamp(sinceWhen);
      String query = 
        "SELECT LATITUDE, LONGITUDE, TIME FROM DEVICECOORDINATES WHERE " + 
        "(TIME > timestamp'" + ts.toString() + "' AND UNIQUEID = '" + 
        RFIDNum + "') " + "ORDER BY TIME ASC";
      Statement stmt = conn.createStatement();
      System.out.println(query);
      ResultSet result = stmt.executeQuery(query);

      while (result.next())
      {
        String lat = result.getString("LATITUDE");
        Float latVal = Float.valueOf(lat.substring(0, 2));
        latVal += Float.valueOf(lat.substring(3, 9)) / 60.0;
        latVal = 
            lat.substring(lat.length() - 1).equals("N")? latVal: -1 * latVal;
        results.add(latVal.toString());

        String longi = result.getString("LONGITUDE");
        Float longVal = Float.valueOf(longi.substring(0, 3));
        longVal += Float.valueOf(longi.substring(4, 10)) / 60.0;
        longVal = 
            longi.substring(longi.length() - 1).equals("W")? -1 * longVal: 
            longVal;
        results.add(longVal.toString());

        Timestamp ts1 = result.getTimestamp("TIME");
        GregorianCalendar cal = new GregorianCalendar();
        cal.setTime(ts1);
        cal.add(Calendar.SECOND, 1);
        String currentDate = 
          months[cal.get(Calendar.MONTH)] + ", " + cal.get(Calendar.DAY_OF_MONTH) + 
          " " + cal.get(Calendar.YEAR) + " " + 
          cal.get(Calendar.HOUR_OF_DAY) + ":" + cal.get(Calendar.MINUTE) + 
          ":" + cal.get(Calendar.SECOND);
        results.add(currentDate);
      }
    }
    catch (SQLException e)
    {
      System.out.println("Exception" + e.toString() + e.getMessage());
      // TODO
    }

    return (String[]) results.toArray(new String[results.size()]);
  }

  @WebMethod
  public boolean setItem(itemInfo newItem, String UID)
  {
    OracleDataSource ods;
    try
    {
      ods = new OracleDataSource();
      ods.setURL(connectString);
      Connection conn = ods.getConnection();
      conn.setAutoCommit(false);
      // make the matches case insensitive
      OracleStatement psC = (OracleStatement) conn.createStatement();
      psC.execute("alter session set NLS_COMP=ANSI");
      psC.execute("alter session set NLS_SORT =BINARY_CI");

      psC.close();

      // first insert the ID
      PreparedStatement ps = 
        conn.prepareStatement("INSERT INTO AUTHENTICATED VALUES (?, ?)");

      ps.setString(1, newItem.getRFIDNum());
      if (newItem.isAuthenticated())
        ps.setString(2, UID);
      else
        ps.setString(2, "");
      ps.execute();

      // then add authentication and tell who authorized it
      ps = conn.prepareStatement("INSERT INTO DESCRIPTIONS VALUES (?, ?)");

      ps.setString(1, newItem.getRFIDNum());
      ps.setString(2, newItem.getDescription());

      ps.execute();

      conn.commit();
      return true;
    }
    catch (SQLException e)
    {
      System.out.println("Exception" + e.toString());
      return false;
      // TODO
    }
  }


  @WebMethod
  public itemInfo getItem(String RFIDNum, String UID, float lat, 
                          float longit)
  {
    if (RFIDNum == null || UID == null)
      return new itemInfo();
    if (RFIDNum.length() > 16 || UID.length() > 128)
    {
      System.out.println("Value too long.");
      return new itemInfo();
    }
    OracleDataSource ods;
    try
    {
      RFIDNum = RFIDNum.replaceAll("\\s+", "");
      for (int i = 0; i < RFIDNum.length(); i++)
        if (!isHexStringChar(RFIDNum.charAt(i)))
          return new itemInfo();

      ods = new OracleDataSource();
      ods.setURL(connectString);
      Connection conn = ods.getConnection();
      conn.setAutoCommit(false);

      // first record this
      PreparedStatement ps = 
        conn.prepareStatement("INSERT INTO AUTHENTICATIONLOOKUPS VALUES (?, ?, ?, ?, ?)");

      java.util.Date today = new java.util.Date();
      ps.setString(1, UID);
      ps.setString(2, RFIDNum);
      ps.setFloat(3, lat);
      ps.setFloat(4, longit);
      ps.setTimestamp(5, new Timestamp(today.getTime()));

      System.out.println("Lookup: " + RFIDNum);
      ps.execute();
      ps.close();
      // then see if it's authenticated
      ps = 
          conn.prepareStatement("SELECT * FROM DESCRIPTIONS WHERE ID = ?");
      ps.setString(1, RFIDNum);

      ResultSet result = ps.executeQuery();

      boolean authenticated;
      String description;

      if (result.next())
      {
        description = result.getString("DESCRIPTION");
        if (result.getInt("AUTHENTICATED") == 1)
          authenticated = true;
        else
          authenticated = false;
      }
      else
      {
        authenticated = false;
        description = "No description found.";
      }

      result.close();
      ps.close();

      conn.commit();
      conn.close();

      return new itemInfo(RFIDNum, description, authenticated);
    }
    catch (SQLException e)
    {
      System.out.println("Exception" + e.toString());
      return new itemInfo();
    }
  }

  /**
   * Checks the character to see if it is hexadecimal
   * @param c
   * @return
   */
  private static final boolean isHexStringChar(char c)
  {
    return (Character.isDigit(c) || 
            (("0123456789abcdefABCDEF".indexOf(c)) >= 0));
  }

  /**
   * 
   * @return
   */
  @WebMethod
  public String[] getUniqueUIDs()
  {
    OracleDataSource ods;

    try
    {
      ods = new OracleDataSource();
      ods.setURL(connectString);
      Connection conn = ods.getConnection();
      Statement stmt = conn.createStatement();
      ResultSet results = 
        stmt.executeQuery("SELECT DISTINCT UNIQUEID FROM DEVICECOORDINATES");
      System.out.println("SELECT DISTINCT UNIQUEID FROM DEVICECOORDINATES");
      ArrayList uniqueIDs = new ArrayList();
      while (results.next())
      {
        uniqueIDs.add(results.getString("UNIQUEID"));
      }
      System.out.println(uniqueIDs.size() + uniqueIDs.get(0).toString());
      return (String[]) uniqueIDs.toArray(new String[uniqueIDs.size()]);
    }
    catch (SQLException e)
    {
      System.out.println("Exception" + e.toString());
      return null;
      // TODO
    }
  }

  /**
   * 
   * @param UID
   * @param lat
   * @param longit
   * @param timeSinceLastReading
   * @param reportedTime
   * @param bearing
   * @param speed
   * @param elevation
   * @return
   */
  @WebMethod
  public boolean callHome(String UID, float[] lat, float[] longit, 
                          long[] timeSinceLastReading, long[] reportedTime, 
                          float[] bearing, float[] speed, int[] elevation)
  {
    OracleDataSource ods;

    try
    {
      ods = new OracleDataSource();
      ods.setURL(connectString);
      Connection conn = ods.getConnection();
      Statement stmt = conn.createStatement();
      conn.setAutoCommit(false);
      PreparedStatement ps = 
        conn.prepareStatement("INSERT INTO DEVICECOORDINATES2 VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
      ps.setString(1, UID);
      ps.setInt(7, 0);
      ps.setTimestamp(4, new Timestamp((new java.util.Date()).getTime()));

      for (int j = 0; j < lat.length; j++)
      {
        ps.setFloat(2, lat[j]);
        ps.setFloat(3, longit[j]);
        ps.setTimestamp(6, new Timestamp(reportedTime[j]));
        ps.setLong(5, timeSinceLastReading[j]);
        ps.setFloat(8, bearing[j]);
        ps.setFloat(9, speed[j]);
        ps.setInt(10, elevation[j]);
        ps.addBatch();
      }
      int[] finishedCounts = ps.executeBatch();
      conn.commit();

      return true;
    }
    catch (SQLException e)
    {
      System.out.println("Exception " + e.toString());
      return false;
      // TODO
    }
  }

  private static String[] months =
  { "January", "February", "March", "April", "May", "June", "July", 
    "August", "September", "October", "November", "December" };


  @WebMethod
  public String[] getSince(long timestamp)
  {
    OracleDataSource ods;
    ArrayList results = new ArrayList();
    try
    {
      ods = new OracleDataSource();
      ods.setURL(connectString);
      System.out.println(connectString);
      Connection conn = ods.getConnection();
      System.out.println("Connected to " + connectString);
      Timestamp ts = new Timestamp(timestamp);
      String query = 
        "SELECT * FROM AUTHENTICATIONLOOKUPS WHERE" + "(TIME > timestamp'" + 
        ts.toString() + "') " + "ORDER BY TIME ASC";
      Statement stmt = conn.createStatement();
      System.out.println(query);
      ResultSet result = stmt.executeQuery(query);

      while (result.next())
      {
        results.add(result.getString("UID"));
        results.add(result.getString("TAGID"));
        Timestamp ts1 = result.getTimestamp("TIME");
        GregorianCalendar cal = new GregorianCalendar();
        cal.setTime(ts1);
        cal.add(Calendar.SECOND, 1);
        String currentDate = 
          months[cal.get(Calendar.MONTH)] + ", " + cal.get(Calendar.DAY_OF_MONTH) + 
          " " + cal.get(Calendar.YEAR) + " " + 
          cal.get(Calendar.HOUR_OF_DAY) + ":" + cal.get(Calendar.MINUTE) + 
          ":" + cal.get(Calendar.SECOND);
        results.add(currentDate);
        results.add(result.getString("LATITUDE"));
        results.add(result.getString("LONGITUDE"));
      }
    }
    catch (SQLException e)
    {
      System.out.println("Exception" + e.toString());
      // TODO
    }
    //    String [] results2 = new String[] (results.size());
    //    for (int j = 0; j < results.size(); j++)
    //      results2[j] = results[j].

    return (String[]) results.toArray(new String[results.size()]);
  }

}
