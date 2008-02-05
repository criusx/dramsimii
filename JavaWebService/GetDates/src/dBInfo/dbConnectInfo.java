package dBInfo;

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
}
