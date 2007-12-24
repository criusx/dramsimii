package petPackage;

import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.GraphicsConfiguration;
import java.awt.GraphicsDevice;
import java.awt.GraphicsEnvironment;
import java.awt.HeadlessException;
import java.awt.Image;
import java.awt.Transparency;
import java.awt.image.BufferedImage;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;


import java.sql.Blob;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Timestamp;

import oracle.jdbc.driver.OracleConnection;
import oracle.jdbc.driver.OraclePreparedStatement;
import oracle.jdbc.driver.OracleResultSet;
import oracle.jdbc.driver.OracleStatement;
import oracle.jdbc.pool.OracleDataSource;

import sun.misc.BASE64Decoder;
import sun.misc.BASE64Encoder;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

import java.sql.Statement;
import java.sql.Timestamp;

import java.util.Arrays;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;

import java.util.Random;

import javax.jws.WebService;

import oracle.jdbc.OracleParameterMetaData;
import oracle.jdbc.driver.OracleConnection;
import oracle.jdbc.driver.OracleResultSet;
import oracle.jdbc.driver.OracleStatement;
import oracle.jdbc.pool.OracleDataSource;
import oracle.jdbc.driver.*;

import oracle.sql.BLOB;

import java.security.SecureRandom;

import java.sql.SQLWarning;

import javax.imageio.ImageIO;

import javax.swing.ImageIcon;


public class petService
{
  private static final String connectString = 
    "jdbc:oracle:thin:rfid/rfid2006@192.168.10.10:1521:orcl1";

  private static final int iterationNb = 1024;

  public errorMessage authorize(String RFIDNum)
  {
    OracleDataSource ods;
    try
    {
      ods = new OracleDataSource();
      ods.setURL(connectString);
      OracleConnection conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      // first check to see if it exists and needs to be updated
      PreparedStatement ps = 
        conn.prepareStatement("SELECT * FROM PETDESCRIPTIONS WHERE ID = ?");

      ps.setString(1, RFIDNum);

      ResultSet rs = ps.executeQuery();

      SQLWarning x = ps.getWarnings();

      if (x != null)
        System.out.println(x.getMessage());

      x = rs.getWarnings();

      if (x != null)
        System.out.println(x.getMessage());

      // if this ID already exists
      if (rs.next())
      {
        rs.close();
        ps.close();

        return new errorMessage("ID already exists in database");
      }
      else
      {
        rs.close();
        ps.close();
        // generate the salt
        SecureRandom random = SecureRandom.getInstance("SHA1PRNG");
        byte[] salt = new byte[8];
        random.nextBytes(salt);
        // create the digest
        byte[] defaultPasswordArray = new byte[8];
        random.nextBytes(defaultPasswordArray);
        String defaultPassword = byteToBase64(defaultPasswordArray);
        // TODO: generate random password
        byte[] digest = getHash(iterationNb, defaultPassword, salt);
        String plainDigest = byteToBase64(digest);
        String plainSalt = byteToBase64(salt);

        byte[] tagCodeDigest = getHash(iterationNb, RFIDNum, salt);
        String tagCode = byteToBase64(tagCodeDigest).substring(0, 8);
        byte[] newDigest = getHash(iterationNb, defaultPassword, salt);
        ps = 
            conn.prepareStatement("INSERT INTO PETDESCRIPTIONS (ID, TAGCODE, PASSWORD, SALT) VALUES (?, ?, ?, ?)");
        ps.setString(1, RFIDNum);
        ps.setString(2, tagCode);
        ps.setString(3, plainDigest);
        ps.setString(4, plainSalt);
        ps.execute();
        rs.close();
        ps.close();
        conn.commit();
        conn.close();
        return new errorMessage("Success", tagCode, defaultPassword);
      }
    }
    catch (SQLException e)
    {
      System.out.println(e.toString());
      return new errorMessage(e.toString());
    }
    catch (NoSuchAlgorithmException exc)
    {
      System.out.println(exc.toString());
      return new errorMessage(exc.toString());
    }
    catch (NullPointerException e)
    {
      System.out.println(e.toString());
      return new errorMessage(e.toString());
    }
  }

  /**
   * Change the password based on the tag code
   * @param tagCode
   * @param oldPassword
   * @param newPassword
   * @return
   */
  public boolean changePasswordTagCode(String tagCode, String oldPassword, 
                                       String newPassword)
  {
    if (tagCode == null)
      return false;

    tagCode = tagCode.replaceAll("\\s+", "");

    // if a non hex string is entered, don't try any db stuff on it
    //    for (int i = 0; i < phoneID.length(); i++)
    //      if (!isHexStringChar(phoneID.charAt(i)))
    //        return new petInfo();
    //
    //    for (int i = 0; i < tagCode.length(); i++)
    //      if (!isHexStringChar(tagCode.charAt(i)))
    //        return new petInfo();

    OracleDataSource ods;
    OracleConnection conn = null;

    boolean result = false;

    try
    {
      ods = new OracleDataSource();
      ods.setURL(connectString);
      conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      OraclePreparedStatement ps = 
        (OraclePreparedStatement) conn.prepareStatement("SELECT ID FROM PETDESCRIPTIONS WHERE TAGCODE = ?");
      ps.setString(1, tagCode);
      ResultSet rs = ps.executeQuery();

      if (rs.next())
      {
        String RFIDNum = rs.getString("ID");
        rs.close();
        ps.close();
        result = changePassword(RFIDNum, oldPassword, newPassword);
      }
      else
      {
        System.out.println("Tag Code not found: " + tagCode);
        rs.close();
        ps.close();
      }
    }
    catch (SQLException e)
    {
      System.out.println(e.toString());
      try
      {
        if (conn != null)
          conn.rollback();
      }
      catch (SQLException ex)
      {
        System.out.println(ex.toString());
        ex.printStackTrace();
      }
    }
    finally
    {
      try
      {
        conn.close();
      }
      catch (SQLException ex)
      {
        System.out.println("Failed to close connection: " + ex.toString());
      }
      return result;
    }
  }

  /**
   * Changes the password for a given RFID Number
   * @param RFIDNum the RFID Number
   * @param oldPassword the old user password
   * @param newPassword the new user password
   * @return true if successful, false if the old password was incorrect
   */
  public boolean changePassword(String RFIDNum, String oldPassword, 
                                String newPassword)
  {
    OracleDataSource ods;
    try
    {
      ods = new OracleDataSource();
      ods.setURL(connectString);
      OracleConnection conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      // first check to see if it exists and needs to be updated
      PreparedStatement ps = 
        conn.prepareStatement("SELECT SALT, PASSWORD FROM PETDESCRIPTIONS WHERE ID = ?", 
                              ResultSet.TYPE_SCROLL_INSENSITIVE, 
                              ResultSet.CONCUR_UPDATABLE);

      ps.setString(1, RFIDNum);

      ResultSet rs = ps.executeQuery();

      SQLWarning x = ps.getWarnings();

      if (x != null)
        System.out.println(x.getMessage());

      x = rs.getWarnings();

      if (x != null)
        System.out.println(x.getMessage());

      if (rs.next())
      {
        // get the salt
        String salt = rs.getString("SALT");
        byte[] saltArray = base64ToByte(salt);

        // get the digest
        String digest = rs.getString("PASSWORD");
        byte[] digestArray = base64ToByte(digest);

        if (salt == null || digest == null)
        {
          System.out.println("Existing entry found with no password. Please notify your system administrator.");
          return false;
        }

        byte[] proposedDigest = 
          getHash(iterationNb, oldPassword, saltArray);

        if (!Arrays.equals(digestArray, proposedDigest))
        {
          System.out.println("Change password failure...");
          conn.rollback();
          return false;
        }
        else
        {
          byte[] newDigest = getHash(iterationNb, newPassword, saltArray);
          rs.updateString("PASSWORD", byteToBase64(newDigest));
          rs.updateRow();
          rs.close();
          ps.close();
          conn.commit();
          conn.close();
          return true;
        }
      }
      else
      {
        System.out.println("Unknown RFID Number tried to change password: " + 
                           RFIDNum);
        ps.close();
        rs.close();
        conn.rollback();
        return false;
      }
    }
    catch (SQLException e)
    {
      System.out.println(e.toString());
      return false;
    }
    catch (IOException ex)
    {
      System.out.println(ex.toString());
      return false;
    }
    catch (NoSuchAlgorithmException exc)
    {
      System.out.println(exc.toString());
      return false;
    }
    catch (NullPointerException e)
    {
      System.out.println(e.toString());
      return false;
    }
  }

  /**
   * Inserts or updates the information of a pet based on its tag code
   * @param newPet
   * @return
   */
  public errorMessage updatePetInformationTagCode(petInfo newPet)
  {
    String tagCode = newPet.getTagCode();
    if (tagCode == null)
      return new errorMessage("");

    tagCode = tagCode.replaceAll("\\s+", "");

    // if a non hex string is entered, don't try any db stuff on it
    //    for (int i = 0; i < phoneID.length(); i++)
    //      if (!isHexStringChar(phoneID.charAt(i)))
    //        return new petInfo();
    //
    //    for (int i = 0; i < tagCode.length(); i++)
    //      if (!isHexStringChar(tagCode.charAt(i)))
    //        return new petInfo();

    OracleDataSource ods;
    OracleConnection conn = null;

    errorMessage eM = null;

    try
    {
      ods = new OracleDataSource();
      ods.setURL(connectString);
      conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      OraclePreparedStatement ps = 
        (OraclePreparedStatement) conn.prepareStatement("SELECT ID FROM PETDESCRIPTIONS WHERE TAGCODE = ?");
      ps.setString(1, tagCode);
      ResultSet rs = ps.executeQuery();

      if (rs.next())
      {
        newPet.setRfidNum(rs.getString("ID"));
        rs.close();
        ps.close();
        eM = updatePetInformation(newPet);
      }
      else
      {
        System.out.println("Tag Code not found: " + tagCode);
        rs.close();
        ps.close();
        eM = new errorMessage("Tag Code not found.");
      }
    }
    catch (SQLException e)
    {
      eM = new errorMessage(e.toString());
      System.out.println(e.toString());
      try
      {
        if (conn != null)
          conn.rollback();
      }
      catch (SQLException ex)
      {
        System.out.println(ex.toString());
        ex.printStackTrace();
      }
    }
    finally
    {
      try
      {
        conn.close();
      }
      catch (SQLException ex)
      {
        System.out.println("Failed to close connection: " + ex.toString());
      }
      if (eM != null)
        return eM;
      else
        return new errorMessage("Error.");
    }
  }

  /**
   * 
   * @param newPet
   * @return
   */
  public errorMessage updatePetInformation(petInfo newPet)
  {
    OracleDataSource ods;
    try
    {
      ods = new OracleDataSource();
      ods.setURL(connectString);
      OracleConnection conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      // first check to see if it exists and needs to be updated
      PreparedStatement ps = 
        conn.prepareStatement("SELECT PETDESCRIPTIONS.* FROM PETDESCRIPTIONS WHERE ID = ?", 
                              ResultSet.TYPE_SCROLL_INSENSITIVE, 
                              ResultSet.CONCUR_UPDATABLE);

      ps.setString(1, newPet.getRfidNum());

      newPet.setImage(resizeImage(newPet.getImage()));

      ResultSet rs = ps.executeQuery();

      SQLWarning x = ps.getWarnings();

      if (x != null)
        System.out.println(x.getMessage());

      x = rs.getWarnings();

      if (x != null)
        System.out.println(x.getMessage());

      if (rs.next())
      {
        //SQLWarning warning = rs.getWarnings();

        //System.out.println("Warn:" + warning.getMessage());
        if (rs.getConcurrency() == ResultSet.CONCUR_READ_ONLY)
          System.out.println("Warn: Updatable resultset not supported");
        //  return new errorMessage("Cannot update entry. Please notify your system administrator.");
        // then the entry existed previously, so check the password
        // get the salt
        String salt = rs.getString("SALT");
        byte[] saltArray = base64ToByte(salt);

        // get the digest
        String digest = rs.getString("PASSWORD");
        byte[] digestArray = base64ToByte(digest);

        if (salt == null || digest == null)
          return new errorMessage("Existing entry found with no password. Please notify your system administrator.");

        byte[] proposedDigest = 
          getHash(iterationNb, newPet.getPassword(), saltArray);

        if (!Arrays.equals(digestArray, proposedDigest))
        {
          System.out.println("Update password failure...");
          conn.rollback();
          return new errorMessage("Incorrect ID or password, cannot update entry.");
        }
        rs.updateString("OWNER", newPet.getOwner());
        rs.updateString("CONTACTINFO", newPet.getContactInfo());
        rs.updateString("BREED", newPet.getBreed());
        rs.updateString("NAME", newPet.getName());
        rs.updateString("HOMEPHONE", newPet.getHomePhone());
        rs.updateString("CELLPHONE", newPet.getCellPhone());
        rs.updateString("WORKPHONE", newPet.getWorkPhone());
        rs.updateString("EMAIL", newPet.getEmail());
        rs.updateString("PREFERREDFOOD", newPet.getPreferredFood());
        rs.updateString("MEDICALNEEDS", newPet.getMedicalNeeds());
        rs.updateString("MEDICATIONS", newPet.getMedications());
        rs.updateString("VETNAME", newPet.getVetName());
        rs.updateString("VETPHONE", newPet.getVetPhone());
        rs.updateString("VETADDRESS", newPet.getVetAddress());
        rs.updateInt("REWARD", newPet.getReward());

        //Blob b = rs.getBlob("PICTURE");
        BLOB b = BLOB.createTemporary(conn, true, BLOB.DURATION_SESSION);

        //b.truncate(0);
        //OutputStream out = b.setBinaryStream(0L);
        //out.write(newPet.getImage(), 0, newPet.getImage().length);
        b.setBytes(1L, newPet.getImage());
        rs.updateBlob("PICTURE", b);

        //rs.next();
        rs.updateRow();
        rs.close();
        ps.close();
      }
      else
      {
        ps.close();
        // else the entry is new, so create it
        rs.close();

        return new errorMessage("This entry is not authorized");
      }

      conn.commit();

      conn.close();
      return new errorMessage();
    }

    catch (SQLException e)
    {
      System.out.println(e.toString());
      return new errorMessage(e.toString());
    }
    catch (IOException ex)
    {
      System.out.println(ex.toString());
      return new errorMessage(ex.toString());
    }
    catch (NoSuchAlgorithmException exc)
    {
      System.out.println(exc.toString());
      return new errorMessage(exc.toString());
    }
    catch (NullPointerException e)
    {
      System.out.println(e.toString());
      return new errorMessage(e.toString());
    }
  }

  private byte[] resizeImage(byte[] originalImage)
    throws IOException
  {
    // adjust the image if necessary
    BufferedImage image = 
      ImageIO.read(new ByteArrayInputStream(originalImage));
    int largestDimension = 
      image.getHeight() > image.getWidth()? image.getHeight(): 
      image.getWidth();

    if (largestDimension > 200)
    {
      float scaleFactor = largestDimension / 200;
      int newWidth = (int) ((float) image.getWidth() / scaleFactor);
      int newHeight = (int) ((float) image.getHeight() / scaleFactor);

      BufferedImage newBI = 
        new BufferedImage(newWidth, newHeight, BufferedImage.TYPE_INT_RGB);

      Graphics2D biContext = newBI.createGraphics();

      biContext.drawImage(image.getScaledInstance(newWidth, newHeight, 
                                                  Image.SCALE_SMOOTH), 0, 
                          0, null);

      ByteArrayOutputStream baos = new ByteArrayOutputStream();

      ImageIO.write(newBI, "png", baos);

      return baos.toByteArray();
    }
    // else no scaling is needed
    else
    {
      return (byte[]) originalImage.clone();
    }
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

  /**
   * Lookup the pet's info based on the tag code printed on the collar
   * @param tagCode
   * @param phoneID
   * @param latitude
   * @param longitude
   * @param password
   * @return
   */
  public petInfo retrievePetInformationTagCode(String tagCode, 
                                               String phoneID, 
                                               float latitude, 
                                               float longitude)
  {
    if (tagCode == null || phoneID == null)
      return new petInfo();

    tagCode = tagCode.replaceAll("\\s+", "");
    phoneID = phoneID.replaceAll("\\s+", "");

    // if a non hex string is entered, don't try any db stuff on it
    //    for (int i = 0; i < phoneID.length(); i++)
    //      if (!isHexStringChar(phoneID.charAt(i)))
    //        return new petInfo();
    //
    //    for (int i = 0; i < tagCode.length(); i++)
    //      if (!isHexStringChar(tagCode.charAt(i)))
    //        return new petInfo();

    OracleDataSource ods;
    OracleConnection conn = null;

    petInfo pet = null;

    try
    {
      ods = new OracleDataSource();
      ods.setURL(connectString);
      conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      OraclePreparedStatement ps = 
        (OraclePreparedStatement) conn.prepareStatement("SELECT ID FROM PETDESCRIPTIONS WHERE TAGCODE = ?");
      ps.setString(1, tagCode);
      ResultSet rs = ps.executeQuery();

      if (rs.next())
      {
        String RFIDNum = rs.getString("ID");
        rs.close();
        ps.close();
        pet = 
            retrievePetInformation(RFIDNum, phoneID, latitude, longitude);
      }
      else
      {
        System.out.println("Tag Code not found: " + tagCode);
        rs.close();
        ps.close();
        pet = new petInfo();
      }

      // ensure that one cannot determine the RFID number from the tag Code easily
      pet.setRfidNum("");
    }
    catch (SQLException e)
    {
      pet = new petInfo();
      System.out.println(e.toString());
      try
      {
        if (conn != null)
          conn.rollback();
      }
      catch (SQLException ex)
      {
        System.out.println(ex.toString());
        ex.printStackTrace();
      }
    }
    finally
    {
      try
      {
        conn.close();
      }
      catch (SQLException ex)
      {
        System.out.println("Failed to close connection: " + ex.toString());
      }
      if (pet != null)
        return pet;
      else
        return new petInfo();
    }
  }

  /**
   * 
   * @param rfidNum needed to identify the tag's ID
   * @param phoneID recorded, the unique ID of the phone
   * @param latitude current latitude
   * @param longitude current longitude
   * @return a petInfo object with relevant fields retrieved if successful
   */
  public petInfo retrievePetInformation(String rfidNum, String phoneID, 
                                        float latitude, float longitude)
  {
    if (rfidNum == null || phoneID == null)
      return new petInfo();

    rfidNum = rfidNum.replaceAll("\\s+", "");
    phoneID = phoneID.replaceAll("\\s+", "");

    // if a non hex string is entered, don't try any db stuff on it
    for (int i = 0; i < phoneID.length(); i++)
      if (!isHexStringChar(phoneID.charAt(i)))
        return new petInfo();

    for (int i = 0; i < rfidNum.length(); i++)
      if (!isHexStringChar(rfidNum.charAt(i)))
        return new petInfo();

    OracleDataSource ods;
    OracleConnection conn = null;

    try
    {
      ods = new OracleDataSource();
      ods.setURL(connectString);
      conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      // enter this information into the db
      java.util.Date today = new java.util.Date();
      Timestamp ts = new Timestamp(today.getTime());
      // TODO: mark whether the lookup was successful or not
      OraclePreparedStatement ps = 
        (OraclePreparedStatement) conn.prepareStatement("INSERT INTO PETAUTHENTICATIONLOOKUPS VALUES (?, ?, ?, ?, ?)");
      ps.setString(1, phoneID);
      ps.setString(2, rfidNum);
      ps.setFloat(3, latitude);
      ps.setFloat(4, longitude);
      ps.setTimestamp(5, ts);
      ps.execute();

      ps = 
          (OraclePreparedStatement) conn.prepareStatement("SELECT * from PETDESCRIPTIONS where ID = ?");
      System.out.println("Pet lookup: " + rfidNum);
      ps.setString(1, rfidNum);
      OracleResultSet rs = (OracleResultSet) ps.executeQuery();


      if (rs.next())
      {
        conn.commit();
        BLOB b = rs.getBLOB("picture");
        byte[] image;
        if (b != null)
          image = b.getBytes((long) 1, (int) b.length());
        else
          image = null;

        petInfo pet = 
          new petInfo(image, rs.getString("contactinfo"), 
                      rs.getString("owner"), rs.getString("workphone"), 
                      rfidNum, rs.getString("homephone"), 
                      rs.getString("cellphone"), rs.getString("email"), 
                      rs.getString("breed"), rs.getString("tagcode"), 
                      rs.getString("name"), rs.getString("preferredfood"), 
                      rs.getString("medicalneeds"), 
                      rs.getString("medications"), rs.getString("vetname"), 
                      rs.getString("vetaddress"), rs.getString("vetphone"), 
                      rs.getInt("reward"));
        conn.close();
        return pet;
      }
      else
      {
        conn.rollback();
        return new petInfo(rfidNum);
      }
    }
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
        return new petInfo(rfidNum);
      }
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
        return new petInfo(rfidNum);
      }
    }
  }

  /**
   * Hashes a password and returns its digest for a given password and salt
   * @param iterationNb The number of times to hash
   * @param password The user's password
   * @param salt The IV or nonce
   * @return
   * @throws NoSuchAlgorithmException
   */
  private byte[] getHash(int iterationNb, String password, byte[] salt)
    throws NoSuchAlgorithmException
  {
    MessageDigest digest = MessageDigest.getInstance("SHA-256");
    digest.reset();
    digest.update(salt);
    byte[] input = digest.digest(password.getBytes());
    for (int i = 0; i < iterationNb; i++)
    {
      digest.reset();
      input = digest.digest(input);
    }
    return input;
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

}
