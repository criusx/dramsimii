package COREMedDemo;

public class drugInfo
{
  private String RFIDNum;
  private String description;
  private byte[] picture;
  private String name;
  private boolean exists;
  
  public drugInfo(boolean _exists)
  {
    exists = false;
  }

  public drugInfo()
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

  public void setDescription(String description)
  {
    this.description = description;
  }

  public String getDescription()
  {
    return description;
  }

  public void setPicture(byte[] picture)
  {
    this.picture = picture;
  }

  public byte[] getPicture()
  {
    return picture;
  }

  public void setName(String name)
  {
    this.name = name;
  }

  public String getName()
  {
    return name;
  }

  public void setExists(boolean exists)
  {
    this.exists = exists;
  }

  public boolean isExists()
  {
    return exists;
  }
}
