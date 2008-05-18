package datespackage;

import dBInfo.dbConnectInfo;

import java.io.Serializable;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.sql.Timestamp;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.GregorianCalendar;

import oracle.jdbc.driver.OracleConnection;
import oracle.jdbc.driver.OraclePreparedStatement;
import oracle.jdbc.driver.OracleResultSet;
import oracle.jdbc.driver.OracleStatement;
import oracle.jdbc.pool.OracleDataSource;

import oracle.sql.BLOB;


//import oracle.webservices.databinding.SOAPElementSerializer;
//import javax.xml.soap.SOAPFactory;

public class GetDates
  implements Serializable
{
  private String name;
  private String desc;
  private boolean authenticated;

  public GetDates()
  {
    authenticated = false;
  }

  public GetDates(boolean _authenticated, String _name, String _desc)
  {
    name = _name;
    authenticated = _authenticated;
    desc = _desc;
  }

  public void setAuthenticated(boolean _authenticated)
  {
    authenticated = _authenticated;
  }

  public boolean isAuthenticated()
  {
    return authenticated;
  }

  public void setName(String val)
  {
    name = val;
  }

  public void setDesc(String val)
  {
    desc = val;
  }

  public String getName()
  {
    return name;
  }

  public String getDesc()
  {
    return desc;
  }

  public boolean checkInteraction(String ID, String drugID)
  {
    OracleDataSource ods;
    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      System.out.println(dbConnectInfo.getConnectInfo());
      OracleConnection conn = (OracleConnection) ods.getConnection();
      System.out.println("Connected to " + dbConnectInfo.getConnectInfo());
      OracleStatement stmt = (OracleStatement) conn.createStatement();
      String query = 
        "SELECT * from interactions where patientid ='" + ID + "' AND drugid='" + 
        drugID + "'";
      System.out.println(query);
      OracleResultSet result = (OracleResultSet) stmt.executeQuery(query);
      if (!result.next())
      {
        System.out.println("No interaction found between " + ID + " and " + 
                           drugID);
        return false;
      }
      else
      {
        System.out.println("Interaction found between " + ID + " and " + 
                           drugID);
        return true;
      }
    }
    catch (SQLException e)
    {
      System.out.println("Exception: " + e.toString());
      return false;
    }
  }


  public byte[] getPicture(String ID, boolean isDrug)
  {
    OracleDataSource ods;
    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      System.out.println(dbConnectInfo.getConnectInfo());
      OracleConnection conn = (OracleConnection) ods.getConnection();
      System.out.println("Connected to " + dbConnectInfo.getConnectInfo());
      OracleStatement stmt = (OracleStatement) conn.createStatement();
      String table = isDrug? "drugdata": "patientdata";
      String id = isDrug? "drugid": "id";
      String query = 
        "SELECT picture FROM " + table + " WHERE " + id + "='" + ID + "'";
      OracleResultSet result = (OracleResultSet) stmt.executeQuery(query);
      if (!result.next())
        return new byte[]
          { 0x00 };
      else
      {
        BLOB resultS = result.getBLOB(1);
        System.out.println("Picture found for drugID: " + ID);
        return resultS.getBytes((long) 1, (int) resultS.length());
      }
    }
    catch (SQLException e)
    {
      System.out.println(e.toString());
      return new byte[]
        { 0x01 };
    }
  }

  public String[] getRoute(String RFIDNum, long sinceWhen)
  {
    OracleDataSource ods;
    ArrayList results = new ArrayList();
    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
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

  public boolean setItem(String RFIDNum, String UID, String description, 
                         boolean authenticated)
  {
    OracleDataSource ods;
    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
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

      ps.setString(1, RFIDNum);
      if (authenticated)
        ps.setString(2, UID);
      else
        ps.setString(2, "");
      ps.execute();

      // then add authentication and tell who authorized it
      ps = conn.prepareStatement("INSERT INTO DESCRIPTIONS VALUES (?, ?)");

      ps.setString(1, RFIDNum);
      ps.setString(2, description);

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


  public GetDates getItem(String RFIDNum, String UID, float lat, 
                          float longit)
  {
    OracleDataSource ods;
    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      Connection conn = ods.getConnection();

      // first record this
      PreparedStatement ps = 
        conn.prepareStatement("INSERT INTO AUTHENTICATIONLOOKUPS2 VALUES (?, ?, ?, ?, ?)");
      java.util.Date today = new java.util.Date();
      Timestamp ts = new Timestamp(today.getTime());
      ps.setTimestamp(5, ts);

      ps.setString(1, UID);
      ps.setString(2, RFIDNum);
      ps.setFloat(3, lat);
      ps.setFloat(4, longit);
      System.out.println("Lookup: " + RFIDNum);
      ps.execute();

      // then see if it's authenticated
      ps = 
          conn.prepareStatement("SELECT * FROM AUTHENTICATED WHERE ID = ?");
      ps.setString(1, RFIDNum);
      ResultSet result = ps.executeQuery();
      boolean authenticated;
      if (result.next())
      {
        if (result.getString("AUTHENTICATED").length() < 16)
          authenticated = false;
        else
          authenticated = true;
      }
      else
        authenticated = false;

      // finally, get its description
      ps = 
          conn.prepareStatement("SELECT description from descriptions WHERE id =?");
      ps.setString(1, RFIDNum);
      result = ps.executeQuery();
      String description;
      if (result.next())
        description = result.getString(1);
      else
        description = "No description found.";

      return new GetDates(authenticated, RFIDNum, description);
    }
    catch (SQLException e)
    {
      System.out.println("Exception" + e.toString());
      return new GetDates();
      // TODO
    }
  }

  public static final boolean isHexStringChar(char c)
  {
    return (Character.isDigit(c) || 
            (("0123456789abcdefABCDEF".indexOf(c)) >= 0));
  }

  public patientInfo getPatientInfo(String rfidNum)
  {
    rfidNum = rfidNum.replaceAll("\\s+", "");
    // if a non hex string is entered, don't try any db stuff on it
    for (int i = 0; i < rfidNum.length(); i++)
    {
      if (!isHexStringChar(rfidNum.charAt(i)))
      {
        return new patientInfo();
      }
    }

    OracleDataSource ods;

    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      OracleConnection conn = (OracleConnection) ods.getConnection();


      PreparedStatement ps = 
        (OraclePreparedStatement) conn.prepareStatement("SELECT FNAME, MNAME, LNAME, DESCRIPTION, PICTURE from PATIENTDATA where ID = ?");
      ps.setString(1, rfidNum);
      OracleResultSet rs = (OracleResultSet) ps.executeQuery();

      if (rs.next())
      {
        BLOB b = rs.getBLOB("picture");

        return new patientInfo(rfidNum, 
                               rs.getString("LNAME") + ", " + rs.getString("FNAME") + 
                               " " + rs.getString("MNAME"), 
                               rs.getString("description"), 
                               b.getBytes((long) 1, (int) b.length()));
      }
      else
      {
        return new patientInfo();
      }
    }
    catch (SQLException e)
    {
      System.out.println(e.toString());
      return new patientInfo();
    }
    //return personArray;
  }


  public String[] getUniqueUIDs()
  {
    OracleDataSource ods;

    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
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


  public boolean callHome(String UID, float[] lat, float[] longit, 
                          long[] timeSinceLastReading, long[] reportedTime, 
                          float[] bearing, float[] speed, int[] elevation)
  {
    OracleDataSource ods;

    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
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

  public static String[] months =
  { "January", "February", "March", "April", "May", "June", "July", 
    "August", "September", "October", "November", "December" };


  public String[] getSince(long timestamp)
  {
    OracleDataSource ods;
    ArrayList results = new ArrayList();
    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      System.out.println(dbConnectInfo.getConnectInfo());
      Connection conn = ods.getConnection();
      System.out.println("Connected to " + dbConnectInfo.getConnectInfo());
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
