package assayPackage;

import dBInfo.dbConnectInfo;

import java.io.IOException;

import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;

import java.sql.SQLException;

import java.sql.SQLWarning;

import java.sql.Timestamp;

import java.util.ArrayList;

import oracle.jdbc.driver.OracleConnection;
import oracle.jdbc.driver.OraclePreparedStatement;
import oracle.jdbc.driver.OracleResultSet;
import oracle.jdbc.pool.OracleDataSource;

import oracle.sql.BLOB;

import sun.misc.BASE64Decoder;
import sun.misc.BASE64Encoder;


public class assayService
{
  public assayInfo retrieveAssayInformation(String rfidNum, String phoneID)
  {
    if (rfidNum == null || phoneID == null)
      return new assayInfo();

    rfidNum = rfidNum.replaceAll("\\s+", "");
    phoneID = phoneID.replaceAll("\\s+", "");

    // if a non hex string is entered, don't try any db stuff on it
    for (int i = 0; i < phoneID.length(); i++)
      if (!isHexStringChar(phoneID.charAt(i)))
        return new assayInfo();

    for (int i = 0; i < rfidNum.length(); i++)
      if (!isHexStringChar(rfidNum.charAt(i)))
        return new assayInfo();

    OracleDataSource ods;
    OracleConnection conn = null;

    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      // get the data, check to see if info for this assay exists
      OraclePreparedStatement ps = 
        (OraclePreparedStatement) conn.prepareStatement("SELECT PICTURE, BEFOREMESSAGE, AFTERMESSAGE, TIMER, DESCRIPTION from ASSAYDESCRIPTION where TESTID = (SELECT TESTID FROM ASSAYLOOKUP WHERE TESTRFID = ?)");
      OraclePreparedStatement ps2 = 
        (OraclePreparedStatement) conn.prepareStatement("SELECT IMAGE FROM ASSAYRESULTPHOTOS WHERE TESTID = (SELECT TESTID FROM ASSAYLOOKUP WHERE TESTRFID = ?) ORDER BY IMAGEID ASC");

      ps.setString(1, rfidNum);
      ps2.setString(1, rfidNum);

      OracleResultSet rs = (OracleResultSet) ps.executeQuery();
      OracleResultSet rs2 = (OracleResultSet) ps2.executeQuery();

      assayInfo info;

      // ensure that both parts exist
      if (rs.next() && rs2.next())
      {
        // generate session ID
        SecureRandom random = SecureRandom.getInstance("SHA1PRNG");
        byte[] sessionIDArray = new byte[8];
        random.nextBytes(sessionIDArray);
        String sessionID = byteToBase64(sessionIDArray);

        // get data
        info = new assayInfo(true);

        BLOB b = rs.getBLOB("picture");
        byte[] image;
        if (b != null)
          image = b.getBytes((long) 1, (int) b.length());
        else
          image = null;

        info.setDescriptionImage(image);
        info.setBeforeMessage(rs.getString("BEFOREMESSAGE"));
        info.setAfterMessage(rs.getString("AFTERMESSAGE"));
        info.setSessionID(sessionID);
        info.setTimer(rs.getInt("TIMER"));

        // get images
        ArrayList results = new ArrayList();
        do
        {
          BLOB bl = rs2.getBLOB("IMAGE");
          byte[] im = null;
          if (bl != null)
            im = bl.getBytes((long) 1, (int) bl.length());

          results.add(new assayImage(im));
        }
        while (rs2.next());

        info.setResultImages((assayImage[]) results.toArray(new assayImage[results.size()]));

        // set this record for later
        ps = 
            (OraclePreparedStatement) conn.prepareStatement("INSERT INTO ASSAYRESULTS (SESSIONID, UNIQUEID, TESTID) SELECT ?, ?, TESTID FROM ASSAYLOOKUP WHERE TESTRFID = ?");
        ps.setString(1, sessionID);
        ps.setString(2, phoneID);
        ps.setString(3, rfidNum);
        ps.execute();
      }
      else
      {
        conn.rollback();
        return new assayInfo();
      }
      // store session ID, UID into db to be filled in later
      //java.util.Date today = new java.util.Date();
      //Timestamp ts = new Timestamp(today.getTime());


      conn.commit();
      conn.close();

      info.setRfidNum(rfidNum);

      return info;
    }
    // TODO
    // catch duplicate entry failure and choose another session ID
    catch (SQLException e)
    {
      System.out.println(e.toString());
      e.printStackTrace();
      try
      {
        if (conn != null)
          conn.rollback();
      }
      catch (SQLException ex)
      {
        System.out.println(ex.toString());
      }
      finally
      {
        return new assayInfo();
      }
    }
    catch (NoSuchAlgorithmException exc)
    {
      System.out.println(exc.toString());
      return new assayInfo();
    }
    catch (NullPointerException e)
    {
      System.out.println(e.toString());
      e.printStackTrace();
      try
      {
        if (conn != null)
          conn.rollback();
      }
      catch (SQLException ex)
      {
        ex.printStackTrace();
        System.out.println(ex.toString());
      }

      finally
      {
        return new assayInfo();
      }
    }
  }

  public boolean submitAssayResult(String sessionID, String phoneID, 
                                   int imageChosen)
  {
    if (sessionID == null || phoneID == null)
      return false;

    sessionID = sessionID.replaceAll("\\s+", "");
    phoneID = phoneID.replaceAll("\\s+", "");

    // if a non hex string is entered, don't try any db stuff on it
    // helps to prevent sql injection attacks
    for (int i = 0; i < phoneID.length(); i++)
      if (!isHexStringChar(phoneID.charAt(i)))
      {
        System.out.println("Rejecting bad Phone UID " + phoneID);
        return false;
      }

    for (int i = 0; i < sessionID.length(); i++)
      if (!isBase64Char(sessionID.charAt(i)))
      {
        System.out.println("Rejecting bad session ID " + sessionID);
        return false;
      }

    OracleDataSource ods;
    OracleConnection conn = null;
    boolean retVal = false;

    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      // get the data, check to see if info for this assay exists
      OraclePreparedStatement ps = 
        (OraclePreparedStatement) conn.prepareStatement("SELECT IMAGECHOSEN, SUBMITTIME from ASSAYRESULTS where SESSIONID = ? AND UNIQUEID = ?", 
                                                        OracleResultSet.TYPE_SCROLL_INSENSITIVE, 
                                                        OracleResultSet.CONCUR_UPDATABLE);

      ps.setString(1, sessionID);
      ps.setString(2, phoneID);

      OracleResultSet rs = (OracleResultSet) ps.executeQuery();

      SQLWarning x = ps.getWarnings();

      if (x != null)
        System.out.println(x.getMessage());

      x = rs.getWarnings();

      if (x != null)
        System.out.println(x.getMessage());

      if (rs.next())
      {      
        rs.updateInt("IMAGECHOSEN", imageChosen);
        rs.updateTimestamp("SUBMITTIME", 
                           new Timestamp((new java.util.Date()).getTime()));
        rs.updateRow();
        retVal = true;
      }
      else
      {
        System.out.println("Unknown session ID: " + sessionID);
        retVal = false;
      }
    }
    // TODO
    // catch duplicate entry failure and choose another session ID
    catch (SQLException e)
    {
      System.out.println(e.toString());
      e.printStackTrace();
    }
    catch (NullPointerException e)
    {
      System.out.println(e.toString());
      e.printStackTrace();
    }
    finally
    {

      try
      {
        if (conn != null)
        {
          if (retVal)
            conn.commit();
          else
            conn.rollback();

          conn.close();
        }
      }
      catch (SQLException ex)
      {
        ex.printStackTrace();
        System.out.println(ex.toString());
      }

    }
    return retVal;
  }

  /**
   * From a base 64 representation, returns the corresponding byte[] 
   * @param data String The base64 representation
   * @return byte[]
   * @throws IOException
   */
  private static byte[] base64ToByte(String data)
    throws IOException
  {
    BASE64Decoder decoder = new BASE64Decoder();
    return decoder.decodeBuffer(data);
  }

  /**
   * From a byte[] returns a base 64 representation
   * @param data byte[]
   * @return String
   * @throws IOException
   */
  private static String byteToBase64(byte[] data)
  {
    BASE64Encoder endecoder = new BASE64Encoder();
    return endecoder.encode(data);
  }

  /**
   * This function tests to see if a given character is a hexadecimal character
   * @param c The input character to test
   * @return true if it is a hex character, false otherwise
   */
  private static final boolean isHexStringChar(char c)
  {
    return (Character.isDigit(c) || 
            (("0123456789abcdefABCDEF".indexOf(c)) >= 0));
  }

  private static final boolean isBase64Char(char c)
  {
    return (Character.isDigit(c) || Character.isLetter(c) || 
            "+=".indexOf(c) >= 0);
  }
}
