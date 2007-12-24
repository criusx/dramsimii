package dhlDemo;

import COREMedDemo.drugInfo;
import COREMedDemo.errorReport;

import java.io.IOException;
import java.io.OutputStream;

import java.sql.Blob;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

import java.sql.Timestamp;

import javax.jws.WebMethod;
import javax.jws.WebService;

import javax.mail.MessagingException;

import oracle.jdbc.driver.OracleConnection;
import oracle.jdbc.pool.OracleDataSource;

import java.util.Random;

import javax.mail.internet.AddressException;

@WebService(name = "DhlDemoWS", serviceName = "DhlDemoWS")
public class dhlDemoService
{
  public dhlDemoService()
  {
  }

  static final String connectString = 
    "jdbc:oracle:thin:rfid/rfid2006@192.168.10.10:1521:orcl1";

  /**
   * Return true if the input argument character is
   * a digit, a space, or A-F.
   */
  public static final boolean isHexStringChar(char c)
  {
    return (Character.isDigit(c) || 
            (("0123456789abcdefABCDEF".indexOf(c)) >= 0));
  }


  @WebMethod
  public packageInfo scanPackage(packageScan newScan)
  {
    OracleDataSource ods;
    try
    {
      ods = new OracleDataSource();
      ods.setURL(connectString);
      OracleConnection conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      packageInfo pI = getPackageInfo(newScan.getRFIDNum());

      PreparedStatement ps = 
          conn.prepareStatement("INSERT INTO PACKAGESCANS (RFID, SCANTIME, LATITUDE, LONGITUDE, TRACKINGNUM, ACTIONTAKEN, SCANNERID) VALUES (?, ?, ?, ?, ?, ?, ?)");
      

      ps.setString(1, newScan.getRFIDNum());
      ps.setTimestamp(2, new Timestamp((new java.util.Date()).getTime()));
      ps.setFloat(3, newScan.getLatitude());
      ps.setFloat(4, newScan.getLongitude());
      ps.setString(5, pI.getTrackingNumber());
      ps.setInt(6, newScan.getActionTaken());
      ps.setString(7, newScan.getScannerID());

      ps.execute();
      
      // then it was delivered
      if (newScan.getActionTaken() == 2)
      {
        // send an email to this address saying when, where and what ID was delivered
        new SendApp().send("smtp.bizland.com", 25, "webmaster@bizland.com", 
                           pI.getRecipientEmail(), 
                           "Your package has been delivered:" + pI.getTrackingNumber(), 
                           "The package bearing the tracking number " + 
                           pI.getTrackingNumber() + 
                           " has been delivered.\n It was delivered to \n" + 
                           pI.getDeliveryAddress() + "\n" + pI.getDeliveryAddress2() + "\nGPS coordinates: " + 
                           Float.toString(newScan.getLatitude()) + "," + 
                           Float.toString(newScan.getLongitude()) + 
                           "\nThe package was delivered at approximately " + 
                           (new Timestamp((new java.util.Date()).getTime())).toString());

      }
      conn.commit();
      return pI;
    }
    catch (SQLException e)
    {
      System.out.println("SQLException " + e.toString());
      
      return new packageInfo();
    }
    catch (MessagingException e)
    {
      System.out.println("MessagingException " + e.toString());
      return new packageInfo();
    }
  }

  @WebMethod
  public packageInfo getPackageInfo(String RFIDNum)
  {
    OracleDataSource ods;
    try
    {
      ods = new OracleDataSource();
      ods.setURL(connectString);
      OracleConnection conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      PreparedStatement ps = 
        conn.prepareStatement("select * from packages where packageordinal = (select max(packageordinal) from packages where rfid = ?)");
      ps.setString(1, RFIDNum);

      ResultSet rS = ps.executeQuery();
      rS.next();
      packageInfo pI = new packageInfo();
      pI.setExists(true);
      pI.setCity(rS.getString("city"));
      pI.setDeliveryAddress(rS.getString("address1"));
      pI.setDeliveryAddress2(rS.getString("address2"));
      pI.setLatitude(rS.getFloat("latitude"));
      pI.setLongitude(rS.getFloat("longitude"));
      pI.setRecipient(rS.getString("recipient"));
      pI.setRecipientEmail(rS.getString("email"));
      pI.setRecipientPager(rS.getString("pager"));
      pI.setRFIDNum(RFIDNum);
      pI.setState(rS.getString("state"));
      pI.setTrackingNumber(rS.getString("trackingnumber"));
      pI.setZip(rS.getString("zip"));

      conn.close();

      return pI;
    }
    catch (SQLException e)
    {
      System.out.println(e.toString());
      System.out.println(e.fillInStackTrace());
      return new packageInfo();
    }
  }

  @WebMethod
  public boolean setPackageInfo(packageInfo newPackage)
  {
    OracleDataSource ods;
    try
    {
      ods = new OracleDataSource();
      ods.setURL(connectString);
      OracleConnection conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      // generate a tracking number
      String trackingNum;
      ResultSet results;
      Random generator = new Random((new java.util.Date()).getTime());
      do
      {
        trackingNum = "";
        trackingNum += (generator.nextInt(9) + 1);
        for (int i = 0; i < 12; i++)
        {
          trackingNum += generator.nextInt(10);
        }
        // check to see if this one is taken
        PreparedStatement check = 
          conn.prepareStatement("select * from packages where trackingnumber = ?");
        check.setString(1, trackingNum);

        results = check.executeQuery();
      }
      while (results.next());


      PreparedStatement ps = 
        conn.prepareStatement("insert into packages " + 
                              "(rfid, trackingnumber, email, pager, address1, packageordinal, recipient, latitude, longitude, country, address2, city, state, zip) " + 
                              "values (?, ?, ?, ?, ?, packageseq.nextval, ?, ?, ?, ?, ?, ?, ?, ?)");

      ps.setString(1, newPackage.getRFIDNum());
      ps.setString(2, trackingNum);
      ps.setString(3, newPackage.getRecipientEmail());
      ps.setString(4, newPackage.getRecipientPager());
      ps.setString(5, newPackage.getDeliveryAddress());
      ps.setString(6, newPackage.getRecipient());
      ps.setFloat(7, newPackage.getLatitude());
      ps.setFloat(8, newPackage.getLongitude());
      ps.setString(9, newPackage.getCountry());
      ps.setString(10, newPackage.getDeliveryAddress2());
      ps.setString(11, newPackage.getCity());
      ps.setString(12, newPackage.getState());
      ps.setString(13, newPackage.getZip());

      ps.execute();

      // if everything worked, then commit
      conn.commit();
      conn.close();
      new SendApp().send("smtp.bizland.com", 25, "webmaster@bizland.com", 
                         newPackage.getRecipientEmail(), 
                         "Your package has been entered: " + trackingNum, 
                         "The package bearing the tracking number " + 
                         trackingNum + " has been entered at " + 
                         (new Timestamp((new java.util.Date()).getTime())).toString() + 
                         ".\n" + "Address:\t" + newPackage.getRecipient() + 
                         "\n\t\t\t" + newPackage.getDeliveryAddress() + 
                         "\n\t\t\t" + newPackage.getDeliveryAddress2() + 
                         "\n\t\t\t" + newPackage.getCity() + ", " + 
                         newPackage.getState() + " " + 
                         newPackage.getZip() + "\n\n" + "(" + 
                         newPackage.getLatitude() + "W," + 
                         newPackage.getLongitude() + "N)");

      return true;
    }
    catch (SQLException e)
    {
      System.out.println(e.toString());
      System.out.println(e.fillInStackTrace());
      return false;
    }
    catch (AddressException e)
    {
      System.out.println(e.toString());
      System.out.println(e.fillInStackTrace());
      return false;
    }
    catch (MessagingException e)
    {
      System.out.println(e.toString());
      System.out.println(e.fillInStackTrace());
      return false;
    }
  }
}
