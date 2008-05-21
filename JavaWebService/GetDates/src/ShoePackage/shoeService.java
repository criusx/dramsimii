package ShoePackage;

import authenticationPackage.itemInfo;

import dBInfo.dbConnectInfo;

import java.sql.Blob;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Timestamp;

import oracle.jdbc.driver.OracleConnection;
import oracle.jdbc.driver.OracleStatement;
import oracle.jdbc.pool.OracleDataSource;

public class shoeService
{
  public itemInfo getItem(String RFIDNum, String UID, float lat, 
                          float longit)
  {
    return getItem(RFIDNum, UID, lat, longit, true);
  }


  public itemInfo getItem(String RFIDNumA, String RFIDNumB, String UID, 
                          float lat, float longit, boolean logThis)
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
}
