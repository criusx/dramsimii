package dhlDemo;

public class packageScan
{
  private String RFIDNum;
  private String scannerID;
  private long scanTime;
  private float latitude;
  private float longitude;
  private int actionTaken;

  public packageScan()
  {
  }

  public void setRFIDNum(String rFIDNum)
  {
    this.RFIDNum = rFIDNum;
  }

  public String getRFIDNum()
  {
    return RFIDNum;
  }

  public void setScanTime(long scanTime)
  {
    this.scanTime = scanTime;
  }

  public long getScanTime()
  {
    return scanTime;
  }

  public void setLatitude(float latitude)
  {
    this.latitude = latitude;
  }

  public float getLatitude()
  {
    return latitude;
  }

  public void setLongitude(float longitude)
  {
    this.longitude = longitude;
  }

  public float getLongitude()
  {
    return longitude;
  }

  public void setActionTaken(int actionTaken)
  {
    this.actionTaken = actionTaken;
  }

  public int getActionTaken()
  {
    return actionTaken;
  }

  public void setScannerID(String scannerID)
  {
    this.scannerID = scannerID;
  }

  public String getScannerID()
  {
    return scannerID;
  }
}
