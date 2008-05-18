package authenticationPackage;

import dBInfo.dbConnectInfo;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.SQLException;

import oracle.jdbc.driver.OracleStatement;
import oracle.jdbc.pool.OracleDataSource;


public class authenticationSecureService
{
  public authenticationSecureService()
  {
  }
  
  
  public boolean setItem(itemInfo newItem, String UID)
  {
    OracleDataSource ods;
    try
    {
      ods = new OracleDataSource();
      ods.setURL(dbConnectInfo.getConnectInfo());
      Connection conn = ods.getConnection();
      conn.setAutoCommit(false);
      // make the matches case insensitive
      OracleStatement psC = (OracleStatement) conn.createStatement();
      psC.execute("alter session set NLS_COMP=ANSI");
      psC.execute("alter session set NLS_SORT =BINARY_CI");

      psC.close();

      // first insert the ID
      PreparedStatement ps = 
        conn.prepareStatement("INSERT INTO AUTHENTICATED VALUES (?, ?)");

      ps.setString(1, newItem.getRFIDNum());
      if (newItem.isAuthenticated())
        ps.setString(2, UID);
      else
        ps.setString(2, "");
      ps.execute();

      // then add authentication and tell who authorized it
      ps = conn.prepareStatement("INSERT INTO DESCRIPTIONS VALUES (?, ?)");

      ps.setString(1, newItem.getRFIDNum());
      ps.setString(2, newItem.getDescription());

      ps.execute();

      conn.commit();
      return true;
    }
    catch (SQLException e)
    {
      System.out.println("Exception" + e.toString());
      return false;
      // TODO
    }
  }

}
