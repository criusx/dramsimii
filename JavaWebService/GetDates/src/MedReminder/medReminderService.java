package MedReminder;

import dBInfo.dbConnectInfo;

import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.image.BufferedImage;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;

import java.sql.Blob;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.SQLWarning;
import java.sql.Timestamp;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Calendar;
import java.util.GregorianCalendar;

import javax.imageio.ImageIO;

import oracle.jdbc.driver.OracleConnection;
import oracle.jdbc.driver.OraclePreparedStatement;
import oracle.jdbc.driver.OracleResultSet;
import oracle.jdbc.driver.OracleStatement;
import oracle.jdbc.pool.OracleDataSource;

import oracle.sql.BLOB;

import petPackage.errorMessage;
import petPackage.petInfo;

import sun.misc.BASE64Decoder;
import sun.misc.BASE64Encoder;


public class medReminderService
{

  private static String[] months =
  { "January", "February", "March", "April", "May", "June", "July", 
    "August", "September", "October", "November", "December" };

  public medReminders getReminders(String login, String password)
  {
    // ignore blank requests
    if (login == null || password == null)
    {
      System.out.println("Null value.");
      return new medReminders();
    }

    // if they can't be real tags anyway
    else if (login.length() < 1 || password.length() < 1)
    {
      System.out.println("Value too short.");
      return new medReminders();
    }

    if (!login.toUpperCase().matches("^[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.(?:[A-Z]{2}|COM|ORG|NET|GOV|MIL|BIZ|INFO|MOBI|NAME|AERO|JOBS|MUSEUM)$"))
    {
      System.out.println("Not a valid email address: " + login);
      return new medReminders();
    }

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

      //      if (logThis)
      //      {
      //        try
      //        {
      //          // first record this
      //          PreparedStatement ps = 
      //            conn.prepareStatement("INSERT INTO SHOESLOOKUPS VALUES (?, ?, ?, ?, ?, ?)");
      //
      //          ps.setString(1, UID);
      //          ps.setString(2, RFIDNumA);
      //          ps.setFloat(3, lat);
      //          ps.setFloat(4, longit);
      //          ps.setTimestamp(5, 
      //                          new Timestamp((new java.util.Date()).getTime()));
      //          ps.setString(6, RFIDNumB);
      //
      //          ps.execute();
      //          ps.close();
      //        }
      //        catch (SQLException e)
      //        {
      //          System.out.println("Insert lookup failed." + e.getErrorCode() + 
      //                             e.getMessage());
      //          e.printStackTrace();
      //        }
      //      }

      userInfo newUser = null;
      // make sure these are the correct credentials
      if ((newUser = checkPassword(login, password, conn)) == null)
      {
        return new medReminders();
      }
      else
      {
        medReminders newItem = new medReminders(newUser);

        // have shown that the user exists, so look for reminder data
        PreparedStatement ps = 
          conn.prepareStatement("SELECT * FROM MEDREMINDERS REMINDER, DRUGDATA DRUGINFO WHERE REMINDER.MED = DRUGINFO.DRUGID AND REMINDER.EMAIL = ? ");

        ps.setString(1, login);

        ResultSet result = ps.executeQuery();

        ArrayList medicines = new ArrayList();
        ArrayList doses = new ArrayList();
        ArrayList lastSents = new ArrayList();
        ArrayList IDs = new ArrayList();
        ArrayList frequencies = new ArrayList();
        GregorianCalendar cal = new GregorianCalendar();

        while (result.next())
        {
          newItem.setName(result.getString("NAME"));

          medicines.add(result.getString("NAME"));

          doses.add(result.getString("DOSE"));

          IDs.add(result.getString("DRUGID"));

          frequencies.add(result.getString("FREQUENCY"));

          cal.setTime(result.getTimestamp("LASTDOSE"));

          lastSents.add(months[cal.get(Calendar.MONTH)] + ", " + 
                        cal.get(Calendar.DAY_OF_MONTH) + " " + 
                        cal.get(Calendar.YEAR) + " " + 
                        cal.get(Calendar.HOUR_OF_DAY) + ":" + 
                        cal.get(Calendar.MINUTE) + ":" + 
                        cal.get(Calendar.SECOND));
        }
        result.close();
        ps.close();

        newItem.setMedicationName((String[]) medicines.toArray(new String[medicines.size()]));
        newItem.setDoses((String[]) doses.toArray(new String[doses.size()]));
        newItem.setLastSent((String[]) lastSents.toArray(new String[lastSents.size()]));
        newItem.setID((String[]) IDs.toArray(new String[IDs.size()]));
        newItem.setReminderFrequency((String[]) frequencies.toArray(new String[frequencies.size()]));

        return newItem;
      }

    }
    catch (SQLException e)
    {
      e.printStackTrace();

      return new medReminders();
    }
    catch (IOException e)
    {
      e.printStackTrace();

      return new medReminders();
    }
    catch (NoSuchAlgorithmException e)
    {
      e.printStackTrace();

      return new medReminders();
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


  public drugInfo getDrug(String RFIDNum, String UID)
  {
    // ignore blank requests
    if (RFIDNum == null || UID == null)
    {
      System.out.println("Null value.");
      return new drugInfo();
    }
    // if they can't be real tags anyway
    if (RFIDNum.length() > 16 || UID.length() > 128)
    {
      System.out.println("Value too long.");
      return new drugInfo();
    }
    // sanitize to help prevent sql injection
    RFIDNum = RFIDNum.replaceAll("\\s+", "");
    // only accept ids in hex
    for (int i = 0; i < RFIDNum.length(); i++)
    {
      if (!isHexChar(RFIDNum.charAt(i)))
      {
        System.out.println("Non-hex character encountered.");
        return new drugInfo();
      }
    }

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

      PreparedStatement ps = 
        conn.prepareStatement("SELECT * FROM DRUGDATA WHERE (DRUGID = (SELECT DRUGID FROM DRUGLOOKUP WHERE DRUGRFID = ?) OR DRUGID = ?)");
      ps.setString(1, RFIDNum);
      ps.setString(2, RFIDNum);

      ResultSet rs = ps.executeQuery();

      // if there is no patient by this id
      if (!rs.next())
      {
        System.out.println("Drug Lookup (failure):" + RFIDNum);
        return new drugInfo(false);
      }
      else
      {
        drugInfo pR = new drugInfo();
        pR.setRFIDNum(RFIDNum);
        pR.setRetryNeeded(false);
        pR.setDescription(rs.getString("DESCRIPTION"));
        pR.setExists(true);
        pR.setAuthenticated(rs.getInt("AUTHENTICATED") == 1);

        pR.setName(rs.getString("NAME"));

        Blob b = rs.getBlob("PICTURE");

        pR.setPicture(b.getBytes((long) 1, (int) b.length()));

        ps.close();

        System.out.println("Drug Lookup (success):" + RFIDNum);

        return pR;
      }
    }
    catch (SQLException e)
    {
      e.printStackTrace();
      return new drugInfo(RFIDNum, true);
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

  private static final int iterationNb = 1024;

  /**
   * Check the password against the stored password to see if they match
   * @param username
   * @param password
   * @param conn
   * @return
   * @throws SQLException
   * @throws IOException
   * @throws NoSuchAlgorithmException
   */
  private userInfo checkPassword(String username, String password, 
                                 OracleConnection conn)
    throws SQLException, IOException, NoSuchAlgorithmException
  {
    // first check to see if it exists and needs to be updated
    PreparedStatement ps = 
      conn.prepareStatement("SELECT * FROM MEDREMINDERUSERS WHERE ID = ?");

    ps.setString(1, username);

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
        rs.close();
        ps.close();
        return null;
      }
      else if (!Arrays.equals(digestArray, 
                              getHash(iterationNb, password, saltArray)))
      {
        System.out.println("Password failure...");
        rs.close();
        ps.close();
        return null;
      }
      else
      {
        userInfo newUser = new userInfo(true);
        newUser.setEmail(username);
        newUser.setPassword("");
        newUser.setName(rs.getString("NAME"));
        newUser.setPhoneNumber(rs.getString("PHONE"));
        newUser.setProvider(rs.getString("PROVIDER"));
        rs.close();
        ps.close();
        return newUser;
      }
    }
    else
    {
      rs.close();
      ps.close();
      return null;
    }
  }

  public errorMessage authorize(userInfo newUser)
  {
    // do error checking
    if (!newUser.getEmail().toUpperCase().matches("^[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.(?:[A-Z]{2}|COM|ORG|NET|GOV|MIL|BIZ|INFO|MOBI|NAME|AERO|JOBS|MUSEUM)$"))
    {
      System.out.println("Not a valid email address: " + 
                         newUser.getEmail());
      return new errorMessage("Invalid email address");
    }

    OracleDataSource ods;
    OracleConnection conn = null;
    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      // first check to see if it exists and needs to be updated
      PreparedStatement ps = 
        conn.prepareStatement("SELECT * FROM MEDREMINDERUSERS WHERE EMAIL = ?");

      ps.setString(1, newUser.getEmail());

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
        //String defaultPassword = byteToBase64(defaultPasswordArray);

        // TODO: generate random password
        //byte[] digest = getHash(iterationNb, defaultPassword, salt);
        byte[] digest = getHash(iterationNb, newUser.getPassword(), salt);
        String plainDigest = byteToBase64(digest);
        String plainSalt = byteToBase64(salt);

        byte[] tagCodeDigest = 
          getHash(iterationNb, newUser.getEmail(), salt);
        String tagCode = byteToBase64(tagCodeDigest).substring(0, 8);

        //byte[] newDigest = getHash(iterationNb, defaultPassword, salt);
        ps = 
            conn.prepareStatement("INSERT INTO MEDREMINDERUSERS " + "(ID, PICTURE, NAME, PHONE, PROVIDER, EMAIL, PASSWORD, SALT, CONFIRMED) " + 
                                  "VALUES (?, empty_blob(), ?, ?, ?, ?, ?, ?, 0)");

        ps.setString(1, newUser.getEmail());
        ps.setString(2, newUser.getName());
        ps.setString(3, newUser.getPhoneNumber());
        ps.setString(4, newUser.getProvider());
        ps.setString(5, newUser.getEmail());
        ps.setString(6, plainDigest);
        ps.setString(7, plainSalt);

        ps.execute();

        ps.close();

        conn.commit();

        return new errorMessage("Success", tagCode, newUser.getPassword());
      }
    }
    catch (SQLException e)
    {
      e.printStackTrace();
      return new errorMessage(e.toString());
    }
    catch (NoSuchAlgorithmException e)
    {
      e.printStackTrace();
      return new errorMessage(e.toString());
    }
    catch (NullPointerException e)
    {
      e.printStackTrace();
      return new errorMessage(e.toString());
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
   * Changes the password for a given RFID Number
   * @param userID the RFID Number
   * @param oldPassword the old user password
   * @param newPassword the new user password
   * @return true if successful, false if the old password was incorrect
   */
  public boolean changePassword(String userID, String oldPassword, 
                                String newPassword)
  {
    OracleDataSource ods;
    OracleConnection conn = null;
    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      // first check to see if it exists and needs to be updated
      PreparedStatement ps = 
        conn.prepareStatement("SELECT SALT, PASSWORD FROM MEDREMINDERUSERS WHERE ID = ?", 
                              ResultSet.TYPE_SCROLL_INSENSITIVE, 
                              ResultSet.CONCUR_UPDATABLE);

      ps.setString(1, userID);

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
        System.out.println("Unknown user ID tried to change password: " + 
                           userID);
        ps.close();
        rs.close();
        conn.rollback();
        conn.close();
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
   * 
   * @param newUser
   * @return
   */
  public errorMessage updateUserInformation(userInfo newUser)
  {
    OracleDataSource ods;
    OracleConnection conn = null;
    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      // first check to see if it exists and needs to be updated
      PreparedStatement ps = 
        conn.prepareStatement("SELECT MEDREMINDERUSERS.* FROM MEDREMINDERUSERS WHERE ID = ?", 
                              ResultSet.TYPE_SCROLL_INSENSITIVE, 
                              ResultSet.CONCUR_UPDATABLE);

      ps.setString(1, newUser.getEmail());

      if (newUser.getImage() != null && newUser.getImage().length > 64)
      {
        newUser.setImage(resizeImage(newUser.getImage()));
      }

      ResultSet rs = ps.executeQuery();

      SQLWarning x = ps.getWarnings();

      if (x != null)
        System.out.println(x.getMessage());

      x = rs.getWarnings();

      if (x != null)
        System.out.println(x.getMessage());

      // if this is an update of existing information
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
          getHash(iterationNb, newUser.getPassword(), saltArray);

        if (!Arrays.equals(digestArray, proposedDigest))
        {
          System.out.println("Update password failure...");
          conn.rollback();
          return new errorMessage("Incorrect ID or password, cannot update entry.");
        }
        rs.updateString("NAME", newUser.getName());
        rs.updateString("PHONE", newUser.getPhoneNumber());
        rs.updateString("PROVIDER", newUser.getProvider());
        rs.updateString("EMAIL", newUser.getEmail());
        rs.updateInt("CONFIRMED", 0);

        //Blob b = rs.getBlob("PICTURE");
        BLOB b = BLOB.createTemporary(conn, true, BLOB.DURATION_SESSION);

        //b.truncate(0);
        //OutputStream out = b.setBinaryStream(0L);
        //out.write(newPet.getImage(), 0, newPet.getImage().length);
        if (newUser.getImage() != null)
        {
          b.setBytes(1L, newUser.getImage());
          rs.updateBlob("PICTURE", b);
        }

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

      return new errorMessage();
    }

    catch (SQLException e)
    {
      System.out.println(e.toString());
      e.printStackTrace();
      return new errorMessage(e.toString());
    }
    catch (IOException e)
    {
      System.out.println(e.toString());
      e.printStackTrace();
      return new errorMessage(e.toString());
    }
    catch (NoSuchAlgorithmException e)
    {
      System.out.println(e.toString());
      e.printStackTrace();
      return new errorMessage(e.toString());
    }
    catch (NullPointerException e)
    {
      System.out.println(e.toString());
      e.printStackTrace();
      return new errorMessage(e.toString());
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
   * resizes an image so it will fit in a given box
   * @param originalImage
   * @return
   * @throws IOException
   */
  private byte[] resizeImage(byte[] originalImage)
    throws IOException
  {
    if (originalImage == null)
      return null;
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
  private static final boolean isHexChar(char c)
  {
    return (Character.isDigit(c) || 
            (("0123456789abcdefABCDEF".indexOf(c)) >= 0));
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
      if (!isHexChar(phoneID.charAt(i)))
        return new petInfo();

    for (int i = 0; i < rfidNum.length(); i++)
      if (!isHexChar(rfidNum.charAt(i)))
        return new petInfo();

    OracleDataSource ods;
    OracleConnection conn = null;

    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
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
          new petInfo(image, rs.getString("contactinfo"), rs.getString("owner"), 
                      rs.getString("workphone"), rfidNum, 
                      rs.getString("homephone"), rs.getString("cellphone"), 
                      rs.getString("email"), rs.getString("breed"), 
                      rs.getString("tagcode"), rs.getString("name"), 
                      rs.getString("preferredfood"), 
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
