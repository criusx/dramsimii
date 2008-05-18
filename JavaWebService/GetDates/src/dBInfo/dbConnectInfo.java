package dBInfo;

import java.sql.SQLException;

import oracle.jdbc.OracleConnection;
import oracle.jdbc.pool.OracleDataSource;


//import oracle.jdbc.driver.*;


public class dbConnectInfo
{
  private static final String connectInfo = 
    "jdbc:oracle:thin:rfid/rfid2006@id1.gentag.com:1521:gentagdb";

  public dbConnectInfo()
  {
  }
  
  public static String getConnectInfo()
  {
    return connectInfo;
  }
  
  public static OracleConnection getConnection() throws SQLException
  {
    OracleDataSource ods = new OracleDataSource();
    ods.setURL(connectInfo);
    OracleConnection conn = (OracleConnection)ods.getConnection();    
    conn.setAutoCommit(false);
    return conn;
  }
}
