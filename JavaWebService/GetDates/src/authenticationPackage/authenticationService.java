package authenticationPackage;

import dBInfo.dbConnectInfo;

import java.io.IOException;
import java.io.OutputStream;

import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;

import java.sql.Blob;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.sql.Timestamp;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.GregorianCalendar;

import javax.jws.WebMethod;
import javax.jws.WebService;

import oracle.jdbc.driver.OracleConnection;
import oracle.jdbc.driver.OracleResultSet;
import oracle.jdbc.driver.OracleStatement;
import oracle.jdbc.pool.OracleDataSource;

import oracle.sql.BLOB;


@WebService(name = "AuthenticationWebService", 
            serviceName = "AuthenticationWebService")
public class authenticationService
{
  public authenticationService()
  {
  }

  @WebMethod
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

  @WebMethod
  public boolean setItem(itemInfo newItem, String UID)
  {
    OracleDataSource ods;
    OracleConnection conn = null;
    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);
      // make the matches case insensitive
      OracleStatement psC = (OracleStatement) conn.createStatement();
      psC.execute("alter session set NLS_COMP=ANSI");
      psC.execute("alter session set NLS_SORT =BINARY_CI");

      psC.close();

      SecureRandom rng = SecureRandom.getInstance("SHA1PRNG");

      int randomID = rng.nextInt();

      // then add authentication and tell who authorized it
      PreparedStatement ps = 
        conn.prepareStatement("INSERT INTO DESCRIPTIONS (ID, DESCRIPTION, AUTHENTICATED, IMAGE) VALUES (?, ?, ?, ?)");

      ps.setString(1, newItem.getRFIDNum());
      ps.setString(2, newItem.getDescription());
      ps.setInt(3, 1);
      ps.setInt(4, randomID);

      ps.execute();

      ps = 
          conn.prepareStatement("INSERT INTO PRODUCTIMAGES (PRODUCTID, PHOTO) VALUES (?, empty_blob())");

      ps.setInt(1, randomID);
      ps.execute();

      ps = 
          conn.prepareStatement("SELECT PHOTO FROM PRODUCTIMAGES WHERE PRODUCTID = ? FOR UPDATE");
      ps.setInt(1, randomID);
      ResultSet rs = ps.executeQuery();
      if (rs.next())
      {
        Blob b = rs.getBlob("PHOTO");

        OutputStream out = b.setBinaryStream(0L);
        out.write(newItem.getImage(), 0, newItem.getImage().length);
        out.flush();

        conn.commit();
        System.out.println("work");
        return true;
      }
      else
      {
        System.out.println("error updating image");
        conn.rollback();
        return false;
      }
    }
    catch (SQLException e)
    {
      e.printStackTrace();
      return false;
      // TODO
    }
    catch (NoSuchAlgorithmException e)
    {
      e.printStackTrace();
      return false;
    }
    catch (IOException e)
    {
      e.printStackTrace();
      return false;
    }
    finally
    {
      // close up the connection
      try
      {
        if (conn != null)
        {
          conn.close();
        }
      }
      catch (SQLException ex)
      {
        ex.printStackTrace();
        try
        {
          if (conn != null)
            conn.close();
        }
        catch (SQLException exc)
        {
          exc.printStackTrace();
        }
      }
    }
  }

  @WebMethod
  public itemInfo getItem(String RFIDNum, String UID, float lat, 
                          float longit)
  {
    return getItem(RFIDNum, UID, lat, longit, true);
  }


  public itemInfo getItem(String RFIDNum, String UID, float lat, 
                          float longit, boolean logThis)
  {
    // ignore blank requests
    if (RFIDNum == null || UID == null)
    {
      System.out.println("Null value.");
      return new itemInfo();
    }
    // if they can't be real tags anyway
    if (RFIDNum.length() < 1)
    {
      System.out.println("Value too short.");
      return new itemInfo();

    }
    if (RFIDNum.length() > 16 || UID.length() > 128)
    {
      System.out.println("Value too long.");
      return new itemInfo();
    }
    // sanitize to help prevent sql injection
    RFIDNum = RFIDNum.replaceAll("\\s+", "");
    // only accept ids in hex
    for (int i = 0; i < RFIDNum.length(); i++)
      if (!isHexStringChar(RFIDNum.charAt(i)))
        return new itemInfo();

    OracleDataSource ods;
    OracleConnection conn = null;

    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      // set case insensitivity
      OracleStatement psC = (OracleStatement) conn.createStatement();
      psC.execute("alter session set NLS_COMP=ANSI");
      psC.execute("alter session set NLS_SORT=BINARY_CI");

      psC.close();
      
      if (logThis)
      {
        try
        {
          // first record this
          PreparedStatement ps = 
            conn.prepareStatement("INSERT INTO AUTHENTICATIONLOOKUPS VALUES (?, ?, ?, ?, ?)");

          ps.setString(1, UID);
          ps.setString(2, RFIDNum);
          ps.setFloat(3, lat);
          ps.setFloat(4, longit);
          ps.setTimestamp(5, 
                          new Timestamp((new java.util.Date()).getTime()));

          ps.execute();
          ps.close();
        }
        catch (SQLException e)
        {
          System.out.println("Insert lookup failed." + e.getErrorCode());
        }
      }

      // then see if it's authenticated
      PreparedStatement ps = 
        conn.prepareStatement("SELECT * FROM DESCRIPTIONS DES, PRODUCTIMAGES PI WHERE DES.IMAGE = PI.PRODUCTID AND ID = ?");
      ps.setString(1, RFIDNum);

      ResultSet result = ps.executeQuery();

      itemInfo newItem = new itemInfo();

      // get the authentication status and description
      if (result.next())
      {
        newItem.setDescription(result.getString("DESCRIPTION"));
        newItem.setAuthenticated(result.getInt("AUTHENTICATED") == 1);
        newItem.setExists(true);

        Blob b = result.getBlob("PHOTO");

        if (b != null)
          newItem.setImage(b.getBytes((long) 1, (int) b.length()));
      }
      else
      {
        newItem.setAuthenticated(false);
        newItem.setExists(false);
        newItem.setDescription("No description found.");
      }

      result.close();

      ps.close();

      conn.commit();
      System.out.println("Lookup: " + RFIDNum + " " + 
                         newItem.isAuthenticated());
      return newItem;
    }
    catch (SQLException e)
    {
      e.printStackTrace();
      if (conn != null)
        conn.rollback();

      return new itemInfo();
    }
    finally
    {
      // close up the connection
      try
      {
        if (conn != null)
        {
          conn.close();
        }
      }
      catch (SQLException ex)
      {
        ex.printStackTrace();
        try
        {
          if (conn != null)
            conn.close();
        }
        catch (SQLException exc)
        {
          exc.printStackTrace();
        }
      }
    }
  }

  @WebMethod
  public boolean sendError(String exception, String stackTrace, 
                           String message)
  {
    // ignore blank requests
    if (exception == null || stackTrace == null)
    {
      System.out.println("Null value.");
      return false;
    }
    // if they can't be real tags anyway
    if (exception.length() < 8 || stackTrace.length() < 16)
    {
      System.out.println("Value too short.");
      return false;
    }
    OracleDataSource ods;
    OracleConnection conn = null;

    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      // first record this
      PreparedStatement ps = 
        conn.prepareStatement("INSERT INTO ERRORREPORTS (EXCEPTION, STACKTRACE, MESSAGE, SUBMITTIME) VALUES (?, ?, ?, ?)");
      ps.setString(1, exception);
      ps.setString(2, stackTrace);
      ps.setString(3, message);
      ps.setTimestamp(4, new Timestamp((new java.util.Date()).getTime()));
      ps.execute();
      ps.close();
      conn.commit();
      return true;
    }
    catch (SQLException e)
    {
      System.out.println("Error report failed." + e.getErrorCode());
      conn.rollback();
      return false;
    }
    finally
    {
      // close up the connection
      try
      {
        if (conn != null)
        {
          conn.close();
        }
      }
      catch (SQLException ex)
      {
        ex.printStackTrace();
        try
        {
          if (conn != null)
            conn.close();
        }
        catch (SQLException exc)
        {
          exc.printStackTrace();
        }
      }
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

  private static String[] months =
  { "January", "February", "March", "April", "May", "June", "July", 
    "August", "September", "October", "November", "December" };


  @WebMethod
  public String[] getSince(long timestamp)
  {
    OracleDataSource ods;
    OracleConnection conn = null;
    ArrayList results = new ArrayList();

    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      conn = (OracleConnection) ods.getConnection();

      PreparedStatement ps = 
        conn.prepareStatement("SELECT * FROM (SELECT * FROM AUTHENTICATIONLOOKUPS WHERE (TIME > ?) ORDER BY TIME ASC) WHERE ROWNUM <=200");
      ps.setTimestamp(1, new Timestamp(timestamp));
      OracleResultSet result = (OracleResultSet) ps.executeQuery();

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

      System.out.println("GetSince: " + timestamp);
      return (String[]) results.toArray(new String[results.size()]);
    }
    catch (SQLException e)
    {
      System.out.println(e.toString());
      e.printStackTrace();
      return new String[1];
    }
    finally
    {
      // close up the connection
      try
      {
        if (conn != null)
        {
          conn.close();
        }
      }
      catch (SQLException ex)
      {
        ex.printStackTrace();
        try
        {
          if (conn != null)
            conn.close();
        }
        catch (SQLException exc)
        {
          exc.printStackTrace();
        }
      }
    }

  }

}
