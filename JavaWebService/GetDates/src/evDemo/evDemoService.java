package evDemo;

import COREMedDemo.drugInfo;
import COREMedDemo.errorReport;

import dhlDemo.packageInfo;
import dhlDemo.packageScan;

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

public class evDemoService
{
  public evDemoService()
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

  public boolean addSpectrum(spectrumInfo newSample)
  {
    OracleDataSource ods;
    try
    {
      ods = new OracleDataSource();
      ods.setURL(connectString);
      OracleConnection conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      PreparedStatement ps = 
        conn.prepareStatement("INSERT INTO SPECTRA (RFIDNUM, BINNUMBER, COUNT) VALUES (?, ?, ?)");
      ps.setString(1, newSample.getRFIDNum());

      for (int i = newSample.getDataArray().length - 1; i >= 0; --i)
      {
        if (newSample.getDataArray()[i] > 0)
        {
          ps.setInt(2,i);
          ps.setInt(3,newSample.getDataArray()[i]);
          ps.addBatch();
        }
      }
      
      int[] insertCount = ps.executeBatch();
      conn.commit();
      return true;
    }
    catch (SQLException e)
    {
      System.out.println("SQLException " + e.toString());

      return false;
    }

  }
  
  public spectrumInfo getSpectrum(String RFIDNum)
  {
    OracleDataSource ods;
    try
    {
      ods = new OracleDataSource();
      ods.setURL(connectString);
      OracleConnection conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      PreparedStatement ps = conn.prepareStatement("SELECT BINNUMBER, COUNT FROM SPECTRA WHERE RFIDNUM=?");
      ps.setString(1,RFIDNum);
      spectrumInfo sI = new spectrumInfo();
      // FIXME: this should not be set to a fixed number
      sI.setDataArray(new int[4096]); 
      ResultSet rs = ps.executeQuery();
      while (rs.next())
      {
        int binNum = rs.getInt("BINNUMBER");
        int count = rs.getInt("COUNT");
        if (binNum < sI.getDataArray().length)
          sI.getDataArray()[binNum] = count;
      }
      
      sI.setExists(true);
      sI.setRFIDNum(RFIDNum);
      
      return sI;
    }
    catch (SQLException e)
    {
      System.out.println("SQLException " + e.toString());

      return new spectrumInfo();
    }
  }
}
