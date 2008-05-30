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
  public shoePair getItem(String RFIDNumA, String RFIDNumB, String UID, 
                          float lat, float longit)
  {
    return getItem(RFIDNumA, RFIDNumB, UID, lat, longit, true);
  }


  public shoePair getItem(String RFIDNumA, String RFIDNumB, String UID, 
                          float lat, float longit, boolean logThis)
  {
    // ignore blank requests
    if (RFIDNumA == null || RFIDNumB == null || UID == null)
    {
      System.out.println("Null value.");
      return new shoePair();
    }

    // if they can't be real tags anyway
    if (RFIDNumA.length() < 1 || RFIDNumB.length() < 1)
    {
      System.out.println("Value too short.");
      return new shoePair();
    }

    if (RFIDNumA.length() > 16 || RFIDNumB.length() > 16 || 
        UID.length() > 128)
    {
      System.out.println("Value too long.");
      return new shoePair();
    }

    // sanitize to help prevent sql injection
    RFIDNumA = RFIDNumA.replaceAll("\\s+", "");
    RFIDNumB = RFIDNumB.replaceAll("\\s+", "");

    // only accept ids in hex
    for (int i = 0; i < RFIDNumA.length(); i++)
      if (!isHexChar(RFIDNumA.charAt(i)))
        return new shoePair();
    for (int i = 0; i < RFIDNumB.length(); i++)
      if (!isHexChar(RFIDNumB.charAt(i)))
        return new shoePair();

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
            conn.prepareStatement("INSERT INTO SHOESLOOKUPS VALUES (?, ?, ?, ?, ?, ?)");

          ps.setString(1, UID);
          ps.setString(2, RFIDNumA);
          ps.setFloat(3, lat);
          ps.setFloat(4, longit);
          ps.setTimestamp(5, 
                          new Timestamp((new java.util.Date()).getTime()));
          ps.setString(6, RFIDNumB);

          ps.execute();
          ps.close();
        }
        catch (SQLException e)
        {
          System.out.println("Insert lookup failed." + e.getErrorCode() + e.getMessage());
          e.printStackTrace();
        }
      }

      // then see if it's authenticated
      PreparedStatement ps = 
        conn.prepareStatement("SELECT * FROM SHOES DES, SHOESIMAGES PI WHERE DES.IMAGE = PI.IMAGE AND ((ID1 = ? AND ID2 = ?) OR (ID2 = ? AND ID1 = ?))");
      ps.setString(1, RFIDNumA);
      ps.setString(2, RFIDNumB);
      ps.setString(3, RFIDNumA);
      ps.setString(4, RFIDNumB);      

      ResultSet result = ps.executeQuery();

      shoePair newItem = new shoePair();

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
      System.out.println("Shoe Lookup: " + RFIDNumA + " " + RFIDNumB + " " + 
                         newItem.isAuthenticated());
      return newItem;
    }
    catch (SQLException e)
    {
      e.printStackTrace();
      if (conn != null)
        conn.rollback();

      return new shoePair();
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
  private static final boolean isHexChar(char c)
  {
    return (Character.isDigit(c) || 
            (("0123456789abcdefABCDEF".indexOf(c)) >= 0));
  }

}
