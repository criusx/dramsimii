package winepackage;

import dBInfo.dbConnectInfo;

import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.image.BufferedImage;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;

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

import javax.imageio.ImageIO;

import oracle.jdbc.driver.OracleConnection;
import oracle.jdbc.driver.OraclePreparedStatement;
import oracle.jdbc.driver.OracleResultSet;
import oracle.jdbc.driver.OracleStatement;
import oracle.jdbc.pool.OracleDataSource;

import oracle.sql.BLOB;


public class wineService
{
  public wineService()
  {
  }

  public boolean enterBottleInformation(String[] rfidNums, String type, 
                                        int year, String country, 
                                        String vineyard, String review, 
                                        byte[] labelImage)
  {
    OracleDataSource ods;
    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      OracleConnection conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      OracleStatement stmt = (OracleStatement) conn.createStatement();

      PreparedStatement ps = 
        conn.prepareStatement("insert into winedescriptions " + 
                              "(id,vineyard,year,type,review,label,country) " + 
                              "values(?, ?, ?, ?, ?, empty_blob(), ?)");

      ps.setString(2, vineyard);
      ps.setInt(3, year);
      ps.setString(4, type);
      ps.setString(5, review);
      ps.setString(6, country);

      PreparedStatement ps2 = 
        conn.prepareStatement("SELECT * FROM winedescriptions WHERE id = ? FOR UPDATE");


      for (String currentId: rfidNums)
      {
        ps.setString(1, currentId);
        ps.execute();
        ps.close();

        ps2.setString(1, currentId);
        ResultSet rs = ps2.executeQuery();

        rs.next();
        Blob b = rs.getBlob("label");
        b.truncate(0);
        OutputStream out = b.setBinaryStream(0L);
        labelImage = resizeImage(labelImage);
        out.write(labelImage, 0, labelImage.length);
        out.flush();
        conn.commit();
      }
      conn.close();
      return true;
    }
    catch (SQLException e)
    {
      System.out.println(e.toString());
      System.out.println(e.fillInStackTrace());
      return false;
    }
    catch (IOException ex)
    {
      System.out.println(ex.toString());
      return false;
    }
  }

  private static final String[] months =
  { "January", "February", "March", "April", "May", "June", "July", 
    "August", "September", "October", "November", "December" };

  public String[] getWineAccessesSince(long timestamp)
  {
    OracleDataSource ods;
    ArrayList results = new ArrayList();
    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      Connection conn = ods.getConnection();
      Timestamp ts = new Timestamp(timestamp);
      String query = 
        "SELECT * FROM WINEAUTHENTICATIONLOOKUPS WHERE" + "(ENTEREDTIME > timestamp'" + 
        ts.toString() + "') " + "ORDER BY ENTEREDTIME ASC";
      Statement stmt = conn.createStatement();
      System.out.println(query);
      ResultSet result = stmt.executeQuery(query);

      while (result.next())
      {
        results.add(result.getString("UID"));
        results.add(result.getString("TAGID"));
        Timestamp ts1 = result.getTimestamp("ENTEREDTIME");
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
    }

    return (String[]) results.toArray(new String[results.size()]);
  }

  /**
   * Return true if the input argument character is
   * a digit, a space, or A-F.
   */
  public static final boolean isHexStringChar(char c)
  {
    return (Character.isDigit(c) || 
            (("0123456789abcdefABCDEF".indexOf(c)) >= 0));
  }

  public wineBottle retrieveBottleInformation(String rfidNum, 
                                              String phoneID, 
                                              float latitude, 
                                              float longitude)
  {
    if (rfidNum == null || phoneID == null)
      return new wineBottle();

    rfidNum = rfidNum.replaceAll("\\s+", "");
    phoneID = phoneID.replaceAll("\\s+", "");

    // if a non hex string is entered, don't try any db stuff on it
    for (int i = 0; i < phoneID.length(); i++)
      if (!isHexStringChar(phoneID.charAt(i)))
        return new wineBottle();

    for (int i = 0; i < rfidNum.length(); i++)
      if (!isHexStringChar(rfidNum.charAt(i)))
        return new wineBottle();

    OracleDataSource ods;
    OracleConnection conn = null;

    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);
      // enter this information into the db
      try
      {
      OraclePreparedStatement ps = 
          (OraclePreparedStatement) conn.prepareStatement("INSERT INTO WINEAUTHENTICATIONLOOKUPS VALUES (?, ?, ?, ?, ?)");
        ps.setString(1, phoneID);
        ps.setString(2, rfidNum);
        ps.setFloat(3, latitude);
        ps.setFloat(4, longitude);
        ps.setTimestamp(5, new Timestamp((new java.util.Date()).getTime()));
        ps.execute();
        
        ps.close();
      }
      catch (SQLException e)
      {
        System.out.println(e.toString());
        e.printStackTrace();
      }

      OraclePreparedStatement ps = 
          (OraclePreparedStatement) conn.prepareStatement("SELECT vineyard, year, type, review, country, label from WINEDESCRIPTIONS where ID = ?");
      
      ps.setString(1, rfidNum);
      OracleResultSet rs = (OracleResultSet) ps.executeQuery();

      ps = 
          (OraclePreparedStatement) conn.prepareStatement("SELECT COUNT(*) FROM WINEAUTHENTICATIONLOOKUPS WHERE TAGID = ?");
      ps.setString(1, rfidNum);
      OracleResultSet rs2 = (OracleResultSet) ps.executeQuery();


      if (rs2.next() && rs.next())
      {
        conn.commit();
        if (rs.next())
        {
          BLOB b = rs.getBLOB("label");

          System.out.println("Wine bottle lookup: " + rfidNum);
          
          return new wineBottle(rs.getNUMBER("year").intValue(), true, 
                                rs.getString("vineyard"), 
                                rs.getString("type"), 
                                rs.getString("country"), 
                                b.getBytes((long) 1, (int) b.length()), 
                                rs.getString("review"), rfidNum, 
                                rs2.getNUMBER(1).intValue(), false);
        }
        else
        {
        // else the number has been looked up multiple times but there is no entry
          return new wineBottle(rfidNum, rs2.getNUMBER(1).intValue());
        }
      }
      else
      {
        conn.rollback();
        return new wineBottle(0, false, "unknown", "unknown", "unknown", 
                              new byte[0], "not available", rfidNum, 0, false);
      }


    }
    catch (SQLException e)
    {
      System.out.println(e.toString());
      e.printStackTrace();
     
        return new wineBottle(0, false, "unknown", "unknown", "unknown", 
                              new byte[0], "not available", rfidNum, 0, true);
      
    }
    catch (NullPointerException e)
    {
      System.out.println(e.toString());
      e.printStackTrace();
        return new wineBottle(0, false, "unknown", "unknown", "unknown", 
                              new byte[0], "not available", rfidNum, 0, true);      
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

  private static final byte[] resizeImage(byte[] originalImage)
    throws IOException
  {
    // adjust the image if necessary
    BufferedImage image = 
      ImageIO.read(new ByteArrayInputStream(originalImage));
    int largestDimension = 
      image.getHeight() > image.getWidth()? image.getHeight(): 
      image.getWidth();

    if (image.getHeight() > 160 || image.getWidth() > 240)
    {
      float heightScale = image.getHeight() / 160;
      float widthScale = image.getHeight() / 240;

      float scaleFactor = 
        heightScale > widthScale? 1 / heightScale: 1 / widthScale;

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
}
