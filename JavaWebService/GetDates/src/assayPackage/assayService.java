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
  public assayInfo retrieveAssayInformation(String RFIDNum, String UID)
  {
    // ignore blank requests
    if (RFIDNum == null || UID == null)
    {
      System.out.println("Null value.");
      return new assayInfo();
    }
    // if they can't be real tags anyway
    if (RFIDNum.length() > 16 || UID.length() > 128)
    {
      System.out.println("Value too long.");
      return new assayInfo();
    }
    // sanitize to help prevent sql injection
    RFIDNum = RFIDNum.replaceAll("\\s+", "");
    // only accept ids in hex
    for (int i = 0; i < RFIDNum.length(); i++)
      if (!isHexStringChar(RFIDNum.charAt(i)))
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

      ps.setString(1, RFIDNum);
      ps2.setString(1, RFIDNum);

      OracleResultSet rs = (OracleResultSet) ps.executeQuery();
      OracleResultSet rs2 = (OracleResultSet) ps2.executeQuery();

      // ensure that both parts exist
      if (rs.next() && rs2.next())
      {
        // generate session ID
        SecureRandom random = SecureRandom.getInstance("SHA1PRNG");
        byte[] sessionIDArray = new byte[8];
        random.nextBytes(sessionIDArray);
        String sessionID = byteToBase64(sessionIDArray);

        // get data
        assayInfo info = new assayInfo(true, false);

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
        ps.setString(2, UID);
        ps.setString(3, RFIDNum);

        ps.execute();

        conn.commit();

        info.setRfidNum(RFIDNum);

        System.out.println("Assay Lookup (success) " + RFIDNum);

        return info;
      }
      else
      {
        conn.rollback();
        return new assayInfo();
      }
    }
    // TODO
    // catch duplicate entry failure and choose another session ID
    catch (SQLException e)
    {
      e.printStackTrace();
      return new assayInfo(false, true);
    }
    catch (NoSuchAlgorithmException exc)
    {
      exc.printStackTrace();
      return new assayInfo(false, true);
    }
    catch (NullPointerException e)
    {
      e.printStackTrace();
      return new assayInfo(false, true);
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

  public assayResultResponse submitAssayResult(String sessionID, 
                                               String phoneID, 
                                               int imageChosen)
  {
    // TODO: make sure image chosen value is within range
    if (sessionID == null || phoneID == null)
    {
      System.out.println("Rejecting assay for null inputs.");
      return new assayResultResponse(false, false);
    }

    sessionID = sessionID.replaceAll("\\s+", "");
    phoneID = phoneID.replaceAll("\\s+", "");

    // if a non hex string is entered, don't try any db stuff on it
    // helps to prevent sql injection attacks
    for (int i = 0; i < phoneID.length(); i++)
      if (!isHexStringChar(phoneID.charAt(i)))
      {
        System.out.println("Rejecting bad Phone UID " + phoneID);
        return new assayResultResponse(false, false);
      }

    for (int i = 0; i < sessionID.length(); i++)
      if (!isBase64Char(sessionID.charAt(i)))
      {
        System.out.println("Rejecting bad session ID " + sessionID);
        return new assayResultResponse(false, false);
      }

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
        rs.close();
        ps.close();
        System.out.println("Assay result sent (success).");
        return new assayResultResponse(true, false);
      }
      else
      {
        System.out.println("Assay result: Unknown session ID: " + 
                           sessionID);
        rs.close();
        ps.close();
        return new assayResultResponse(false, false);
      }
    }
    // TODO
    // catch duplicate entry failure and choose another session ID
    catch (SQLException e)
    {
      System.out.println(e.toString());
      e.printStackTrace();
      return new assayResultResponse(false, true);
    }
    catch (NullPointerException e)
    {
      System.out.println(e.toString());
      e.printStackTrace();
      return new assayResultResponse(false, true);
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
            "+=/".indexOf(c) >= 0);
  }
}
